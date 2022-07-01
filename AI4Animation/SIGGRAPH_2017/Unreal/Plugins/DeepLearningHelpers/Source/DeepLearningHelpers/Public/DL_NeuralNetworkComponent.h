// juaxix - 2020-2022 - xixgames

#pragma once

#include "CoreMinimal.h"

#include "Components/SceneComponent.h"

#include "EigenLibrary.h"
#include "Utility/Tensor.h"
#include "DL_Parameters.h"

#include "DL_NeuralNetworkComponent.generated.h"

using DeepLearningHelpers::Tensor;

UCLASS(Blueprintable, ClassGroup=(AI), meta=(BlueprintSpawnableComponent) , Category = "AI|Deep Learning")
class DEEPLEARNINGHELPERS_API UDL_NeuralNetworkComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UDL_NeuralNetworkComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Neural Network")
	FString Folder = "";

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Neural Network")
	UDL_Parameters* Parameters;

	Tensor *X, *Y;

	int32 Pivot = -1;

	TArray<Tensor> Tensors;

protected:
	virtual void StoreParametersDerived(){};
	virtual void LoadParametersDerived(){};

public:
	UFUNCTION(BlueprintCallable, Category = "AI|Deep Learning|Neural Network")
	virtual void Predict(){}

	UFUNCTION(BlueprintCallable, Category = "AI|Deep Learning|Neural Network")
	virtual void StoreParameters()
	{
		Parameters->Clear();
		StoreParametersDerived();
		
		/*#if WITH_EDITOR
		string[] files = Directory.GetFiles(Folder);
		string directory = new FileInfo(files[0]).Directory.Name;
		Parameters asset = ScriptableObject.CreateInstance<Parameters>();
		string path = AssetDatabase.GenerateUniqueAssetPath("Assets/" + directory + ".asset");
				AssetDatabase.CreateAsset(Parameters, path);
		#endif*/
	}

	UFUNCTION(BlueprintCallable, Category = "AI|Deep Learning|Neural Network")
	virtual void LoadParameters()
	{
		if (Parameters == nullptr)
		{
			UE_LOG(LogProcess, Error, TEXT("Building PFNN failed because no parameters were saved."));
		}
		else
		{
			LoadParametersDerived();
		}
	}

	Tensor* CreateTensor(int32 rows, int32 cols, const FString& id)
	{
		if (Tensors.ContainsByPredicate([id](const Tensor& t) -> bool { return t.ID.Equals(id); }))
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
		if (Tensors.ContainsByPredicate(
			[&matrix](Tensor &t)->bool
			{
				return t.ID == matrix.ID;
			}))
		{
			UE_LOG(LogProcess, Warning, TEXT("Tensor with ID '%s' already contained."), *matrix.ID);
			return nullptr;
		}
		Tensor T(matrix.Rows, matrix.Cols, matrix.ID);
		for(int32 x = 0; x < matrix.Rows; x++)
		{
			for(int32 y = 0; y < matrix.Cols; y++)
			{
				T.SetValue(x, y, matrix.Values[x].Values[y]);
			}
		}
		const int32 i = Tensors.Add(T);
		return &Tensors[i];
	}

	void DeleteTensor(Tensor& T)
	{
		if (!Tensors.Contains(T))
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

	void SetInput(int32 index, float value)
	{
		Pivot = index;
		X->SetValue(index, 0, value);
	}

	float GetInput(int32 index)
	{
		Pivot = index;
		return X->GetValue(index, 0);
	}

	void SetOutput(int32 index, float value)
	{
		Pivot = index;
		Y->SetValue(index, 0, value);
	}

	float GetOutput(int32 index)
	{
		Pivot = index;
		return Y->GetValue(index, 0);
	}

	void Feed(float value)
	{
		SetInput(Pivot + 1, value);
	}

	void Feed(const TArray<float> &values, int32 size)
	{
		for(int32 i = 0; i < size; i++)
		{
			Feed(values[i]);
		}
	}

	void Feed(const FVector2D& vector)
	{
		Feed(vector.X);
		Feed(vector.Y);
	}

	void Feed(const FVector& vector)
	{
		Feed(vector.X);
		Feed(vector.Y);
		Feed(vector.Z);
	}

	float Read()
	{
		return GetOutput(Pivot + 1);
	}

	TArray<float> Read(int32 count)
	{
		TArray<float> values;
		values.Reserve(count);
		for(int32 i = 0; i < count; i++)
		{
			values[i] = Read();
		}
		return values;
	}

	Tensor* Normalise(Tensor* tIN, Tensor* mean, Tensor* std, Tensor* tOUT)
	{
		if (tIN->GetRows() != mean->GetRows() ||
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
			UEigenLibrary::Normalise(tIN->Ptr.Get(), mean->Ptr.Get(), std->Ptr.Get(), tOUT->Ptr.Get());
			return tOUT;
		}
	}

	Tensor* Renormalise(Tensor* tIN, Tensor* mean, Tensor* std, Tensor* tOUT)
	{
		if (tIN->GetRows() != mean->GetRows() ||
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
			UEigenLibrary::Renormalise(tIN->Ptr.Get(), mean->Ptr.Get(), std->Ptr.Get(), tOUT->Ptr.Get());
			return tOUT;
		}
	}

	Tensor* Layer(Tensor* tIN, Tensor* W, Tensor* b, Tensor* tOUT)
	{
		if (tIN->GetRows() != W->GetCols() || W->GetRows() != b->GetRows() ||
			tIN->GetCols() != b->GetCols())
		{
			UE_LOG(LogProcess, Warning, TEXT("Incompatible dimensions for feed-forward."));
			return tIN;
		}
		else
		{
			UEigenLibrary::Layer(tIN->Ptr.Get(), W->Ptr.Get(), b->Ptr.Get(), tOUT->Ptr.Get());
			return tOUT;
		}
	}

	Tensor* Blend(Tensor* T, Tensor* W, float w)
	{
		if (T->GetRows() != W->GetRows() || T->GetCols() != W->GetCols())
		{
			UE_LOG(LogProcess, Warning, TEXT("Incompatible dimensions for blending."));
			return T;
		}

		UEigenLibrary::Blend(T->Ptr.Get(), W->Ptr.Get(), w);
		return T;
	}

	Tensor* ELU(Tensor* T)
	{
		UEigenLibrary::ELU(T->Ptr.Get());
		return T;
	}

	Tensor* Sigmoid(Tensor* T)
	{
		UEigenLibrary::Sigmoid(T->Ptr.Get());
		return T;
	}

	Tensor* TanH(Tensor* T)
	{
		UEigenLibrary::TanH(T->Ptr.Get());
		return T;
	}

	Tensor* SoftMax(Tensor* T)
	{
		UEigenLibrary::SoftMax(T->Ptr.Get());
		return T;
	}

	Tensor* LogSoftMax(Tensor* T)
	{
		UEigenLibrary::LogSoftMax(T->Ptr.Get());
		return T;
	}

	Tensor* SoftSign(Tensor* T)
	{
		UEigenLibrary::SoftSign(T->Ptr.Get());
		return T;
	}


	Tensor* Exp(Tensor* T)
	{
		UEigenLibrary::Exp(T->Ptr.Get());
		return T;
	}
};
