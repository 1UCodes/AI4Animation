// juaxix - 2020-2022 - xixgames

#pragma once

#include "CoreMinimal.h"
#include "DL_NeuralNetworkComponent.h"
#include "DL_PFNN.generated.h"

/**
 *
 */
UCLASS( Blueprintable, ClassGroup=(AI), meta=(BlueprintSpawnableComponent) )
class DEEPLEARNINGHELPERS_API UDL_PFNN : public UDL_NeuralNetworkComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Neural Network")
	int32 XDim = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Neural Network")
	int32 HDim = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Neural Network")
	int32 YDim = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Neural Network")
	int32 PhaseIndex = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Neural Network")
	TArray<int> ControlNeurons;

private:
	Tensor* Xmean;
	Tensor* Xstd;
	Tensor* Ymean;
	Tensor* Ystd;
	TArray<Tensor*> W0;
	TArray<Tensor*> W1;
	TArray<Tensor*> W2;
	TArray<Tensor*> B0;
	TArray<Tensor*> B1;
	TArray<Tensor*> B2;
	float Phase;
	float Damping;

	/**
	 * @TODO: Convert into PostEditorValidate method for Unreal
	 */
	/*void OnValidate()
	{
		ArrayExtensions::Resize(ControlNeurons, XDimBlend);
	}*/

protected:
	void StoreParametersDerived() override
	{
		Parameters->Store(Folder + "/Xmean.bin", XDim, 1, "Xmean");
		Parameters->Store(Folder + "/Xstd.bin", XDim, 1, "Xstd");
		Parameters->Store(Folder + "/Ymean.bin", YDim, 1, "Ymean");
		Parameters->Store(Folder + "/Ystd.bin", YDim, 1, "Ystd");
		for(int32 i = 0; i < 50; i++)
		{
			FString iString = FString::FromInt(i);
			Parameters->Store(Folder + "/W0_" + iString + ".bin", HDim, XDim, "W0_" + iString);
			Parameters->Store(Folder + "/W1_" + iString + ".bin", HDim, HDim, "W1_" + iString);
			Parameters->Store(Folder + "/W2_" + iString + ".bin", YDim, HDim, "W2_" + iString);
			Parameters->Store(Folder + "/b0_" + iString + ".bin", HDim, 1, "b0_" + iString);
			Parameters->Store(Folder + "/b1_" + iString + ".bin", HDim, 1, "b1_" + iString);
			Parameters->Store(Folder + "/b2_" + iString + ".bin", YDim, 1, "b2_" + iString);
		}
	}

	void LoadParametersDerived() override
	{
		Xmean = CreateTensor(Parameters->Load("Xmean"));
		Xstd = CreateTensor(Parameters->Load("Xstd"));
		Ymean = CreateTensor(Parameters->Load("Ymean"));
		Ystd = CreateTensor(Parameters->Load("Ystd"));
		W0.SetNumZeroed(50);
		W1.SetNumZeroed(50);
		W2.SetNumZeroed(50);
		B0.SetNumZeroed(50);
		B1.SetNumZeroed(50);
		B2.SetNumZeroed(50);
		for(int32 i = 0; i < 50; i++)
		{
			FString iString = FString::FromInt(i);
			W0[i] = CreateTensor(Parameters->Load("W0_" + iString));
			W1[i] = CreateTensor(Parameters->Load("W1_" + iString));
			W2[i] = CreateTensor(Parameters->Load("W2_" + iString));
			B0[i] = CreateTensor(Parameters->Load("b0_" + iString));
			B1[i] = CreateTensor(Parameters->Load("b1_" + iString));
			B2[i] = CreateTensor(Parameters->Load("b2_" + iString));
		}

		X = CreateTensor(XDim, 1, "X");
		Y = CreateTensor(YDim, 1, "Y");
		Phase = 0.0f;
		Damping = 0.0f;
	}

public:
	static float RepeatValue(float t, float length)
	{
		return FMath::Clamp(t - FMath::FloorToFloat(t / length) * length, 0.0f, length);
	}

	virtual void Predict() override
	{
		//Normalise Input
		Normalise(X, Xmean, Xstd, Y);

		//Process PFNN
		const int32 index = static_cast<int32>((Phase / (2.f * PI)) * 50.f);
		ELU(Layer(Y, W0[index], B0[index], Y));
		ELU(Layer(Y, W1[index], B1[index], Y));
		Layer(Y, W2[index], B2[index], Y);

		//Renormalise Output
		Renormalise(Y, Ymean, Ystd, Y);

		//Update Phase
		Phase = RepeatValue(
			Phase + (1.f - Damping) * GetOutput(PhaseIndex) * 2.f * PI,
			2.f * PI);
	}

	void SetDamping(float value)
	{
		Damping = value;
	}

	float GetPhase() const
	{
		return Phase / (2.f*PI);
	}
};
