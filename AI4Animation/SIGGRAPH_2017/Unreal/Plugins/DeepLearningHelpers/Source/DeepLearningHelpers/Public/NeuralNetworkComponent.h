// juaxix - 2020 - xixgames

#pragma once

#include "CoreMinimal.h"

#include "Components/SceneComponent.h"
#include "Tensor.h"
#include "EigenLibrary.h"
#include "Parameters.h"

#include "NeuralNetworkComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPLEARNINGHELPERS_API UNeuralNetworkComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UNeuralNetworkComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Neural Network")
	FString Folder = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Neural Network")
	UParameters* Parameters;

	Tensor X, Y;

	int Pivot = -1;

	TArray<Tensor> Tensors;

protected:
	virtual void StoreParametersDerived(){};
	virtual void LoadParametersDerived(){};

public:
	virtual void Predict(){};

	void StoreParameters()
	{
		//Parameters = ScriptableObject.CreateInstance<Parameters>();
		StoreParametersDerived();


		/*#if WITH_EDITOR
		string[] files = Directory.GetFiles(Folder);
		string directory = new FileInfo(files[0]).Directory.Name;
		Parameters asset = ScriptableObject.CreateInstance<Parameters>();
		string path = AssetDatabase.GenerateUniqueAssetPath("Assets/" + directory + ".asset");
				AssetDatabase.CreateAsset(Parameters, path);
		#endif*/
	}

	void LoadParameters()
	{
		if(Parameters == nullptr)
		{
			UE_LOG(LogProcess, Error, TEXT("Building PFNN failed because no parameters were saved."));
		}
		else
		{
			LoadParametersDerived();
		}
	}

	Tensor* CreateTensor(int rows, int cols, const FString& id)
	{
		if(Tensors.ContainsByPredicate([id](Tensor& t) { return t.ID.Equals(id); }))
		{
			UE_LOG(LogProcess, Warning, TEXT("Tensor with ID '%s' already contained."), *id);
			return nullptr;
		}
		const Tensor T(rows, cols, id);
		const int32 i = Tensors.Add(T);
		return &Tensors[i];
	}


	Tensor* CreateTensor(const FPMatrix& matrix)
	{
		if(Tensors.ContainsByPredicate(
			[&matrix](Tensor &t)->bool
			{
				return t.ID == matrix.ID;
			}))
		{
			UE_LOG(LogProcess, Warning, TEXT("Tensor with ID '%s' already contained."), *matrix.ID);
			return nullptr;
		}
		Tensor T(matrix.Rows, matrix.Cols, matrix.ID);
		for(int x = 0; x < matrix.Rows; x++)
		{
			for(int y = 0; y < matrix.Cols; y++)
			{
				T.SetValue(x, y, matrix.Values[x].Values[y]);
			}
		}
		const int32 i = Tensors.Add(T);
		return &Tensors[i];
	}

	void DeleteTensor(Tensor& T)
	{
		if(!Tensors.Contains(T))
		{
			UE_LOG(LogProcess, Warning, TEXT("Tensor with ID '%s' not found."), *T.ID);
			return;
		}

		T.Delete();
		Tensors.RemoveAll([T](auto t){ return t.ID.Equals(T.ID);});
	}

	Tensor* GetTensor(const FString& id)
	{
		return Tensors.FindByPredicate([id](auto t) { return t.ID.Equals(id); });
	}

	const FString& GetID(Tensor* T) const
	{
		if (T && Tensors.Contains(*T))
		{
			return T->ID;
		}
		static FString NotFound = "";
		return NotFound;
	}

	void ResetPivot()
	{
		Pivot = -1;
	}

	void SetInput(int index, float value)
	{
		Pivot = index;
		X.SetValue(index, 0, value);
	}

	float GetInput(int index)
	{
		Pivot = index;
		return X.GetValue(index, 0);
	}

	void SetOutput(int index, float value)
	{
		Pivot = index;
		Y.SetValue(index, 0, value);
	}

	float GetOutput(int index)
	{
		Pivot = index;
		return Y.GetValue(index, 0);
	}

	void Feed(float value)
	{
		SetInput(Pivot + 1, value);
	}

	void Feed(float values[], int size)
	{
		for(int i = 0; i < size; i++)
		{
			Feed(values[i]);
		}
	}

	void Feed(FVector2D& vector)
	{
		Feed(vector.X);
		Feed(vector.Y);
	}

	void Feed(FVector& vector)
	{
		Feed(vector.X);
		Feed(vector.Y);
		Feed(vector.Z);
	}

	float Read()
	{
		return GetOutput(Pivot + 1);
	}

	TArray<float> Read(int count)
	{
		TArray<float> values;
		values.Reserve(count);
		for(int i = 0; i < count; i++)
		{
			values[i] = Read();
		}
		return values;
	}

	Tensor* Normalise(Tensor* tIN, Tensor* mean, Tensor* std, Tensor* tOUT)
	{
		if(tIN->GetRows() != mean->GetRows() ||
			tIN->GetRows() != std->GetRows() ||
			tIN->GetCols() != mean->GetCols() ||
			tIN->GetCols() != std->GetCols()
		)
		{
			UE_LOG(LogProcess, Warning, TEXT("Incompatible dimensions for normalisation."));
			return tIN;
		}
		else
		{
			UEigenLibrary::Normalise(tIN->Ptr, mean->Ptr, std->Ptr, tOUT->Ptr);
			return tOUT;
		}
	}

	Tensor* Renormalise(Tensor* tIN, Tensor* mean, Tensor* std, Tensor* tOUT)
	{
		if(tIN->GetRows() != mean->GetRows() ||
			tIN->GetRows() != std->GetRows() ||
			tIN->GetCols() != mean->GetCols() ||
			tIN->GetCols() != std->GetCols()
		)
		{
			UE_LOG(LogProcess, Warning, TEXT("Incompatible dimensions for renormalisation."));
			return tIN;
		}
		else
		{
			UEigenLibrary::Renormalise(tIN->Ptr, mean->Ptr, std->Ptr, tOUT->Ptr);
			return tOUT;
		}
	}

	Tensor* Layer(Tensor* tIN, Tensor* W, Tensor* b, Tensor* tOUT)
	{
		if(tIN->GetRows() != W->GetCols() || W->GetRows() != b->GetRows() ||
			tIN->GetCols() != b->GetCols())
		{
			UE_LOG(LogProcess, Warning, TEXT("Incompatible dimensions for feed-forward."));
			return tIN;
		}
		else
		{
			UEigenLibrary::Layer(tIN->Ptr, W->Ptr, b->Ptr, tOUT->Ptr);
			return tOUT;
		}
	}

	Tensor* Blend(Tensor* T, Tensor* W, float w)
	{
		if(T->GetRows() != W->GetRows() || T->GetCols() != W->GetCols())
		{
			UE_LOG(LogProcess, Warning, TEXT("Incompatible dimensions for blending."));
			return T;
		}

		UEigenLibrary::Blend(T->Ptr, W->Ptr, w);
		return T;
	}

	Tensor* ELU(Tensor* T)
	{
		UEigenLibrary::ELU(T->Ptr);
		return T;
	}

	Tensor* Sigmoid(Tensor* T)
	{
		UEigenLibrary::Sigmoid(T->Ptr);
		return T;
	}

	Tensor* TanH(Tensor* T)
	{
		UEigenLibrary::TanH(T->Ptr);
		return T;
	}

	Tensor* SoftMax(Tensor* T)
	{
		UEigenLibrary::SoftMax(T->Ptr);
		return T;
	}

	Tensor* LogSoftMax(Tensor* T)
	{
		UEigenLibrary::LogSoftMax(T->Ptr);
		return T;
	}

	Tensor* SoftSign(Tensor* T)
	{
		UEigenLibrary::SoftSign(T->Ptr);
		return T;
	}


	Tensor* Exp(Tensor* T)
	{
		UEigenLibrary::Exp(T->Ptr);
		return T;
	}

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
	override;

};
