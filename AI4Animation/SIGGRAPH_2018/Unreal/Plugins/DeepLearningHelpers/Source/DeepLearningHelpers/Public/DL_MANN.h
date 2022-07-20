// juaxix - 2020-2022 - xixgames

#pragma once

#include "CoreMinimal.h"
#include "DL_NeuralNetworkComponent.h"
#include "DL_MANN.generated.h"

using DeepLearningHelpers::Tensor;

/**
 *
 */
UCLASS(Blueprintable, ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class DEEPLEARNINGHELPERS_API UDL_MANN : public UDL_NeuralNetworkComponent
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
	int32 XDimBlend = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Neural Network")
	int32 HDimBlend = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Neural Network")
	int32 YDimBlend = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Neural Network")
	TArray<int32> ControlNeurons;

private:
	Tensor *Xmean;
	Tensor *Xstd;
	Tensor *Ymean;
	Tensor *Ystd;
	Tensor *BX;
	Tensor *BY;
	Tensor *BW0;
	Tensor *BW1;
	Tensor *BW2;
	Tensor *Bb0;
	Tensor *Bb1;
	Tensor *Bb2;
	TArray<Tensor*> CW;
	Tensor *W0;
	Tensor *W1;
	Tensor *W2;
	Tensor *b0;
	Tensor *b1;
	Tensor *b2;

	/**
	 * @TODO: Convert into PostEditorValidate method for Unreal
	 */
	/*void OnValidate()
	{
		ArrayExtensions::Resize(ControlNeurons, XDimBlend);
	}*/

public:
	void StoreParametersDerived() override
	{
		Parameters->Clear();
		Parameters->Store(Folder/TEXT("Xmean.bin"), XDim, 1, TEXT("Xmean"));
		Parameters->Store(Folder/TEXT("Xstd.bin"), XDim, 1, TEXT("Xstd"));
		Parameters->Store(Folder/TEXT("Ymean.bin"), YDim, 1, TEXT("Ymean"));
		Parameters->Store(Folder/TEXT("Ystd.bin"), YDim, 1, TEXT("Ystd"));
		Parameters->Store(Folder/TEXT("wc0_w.bin"), HDimBlend, XDimBlend, TEXT("wc0_w"));
		Parameters->Store(Folder/TEXT("wc0_b.bin"), HDimBlend, 1, TEXT("wc0_b"));
		Parameters->Store(Folder/TEXT("wc1_w.bin"), HDimBlend, HDimBlend, TEXT("wc1_w"));
		Parameters->Store(Folder/TEXT("wc1_b.bin"), HDimBlend, 1, TEXT("wc1_b"));
		Parameters->Store(Folder/TEXT("wc2_w.bin"), YDimBlend, HDimBlend, TEXT("wc2_w"));
		Parameters->Store(Folder/TEXT("wc2_b.bin"), YDimBlend, 1, TEXT("wc2_b"));
		for(int32 i=0; i < YDimBlend; i++) 
		{
			const FString iString = FString::FromInt(i);
			Parameters->Store(Folder/TEXT("cp0_a")+iString+TEXT(".bin"), HDim, XDim, TEXT("cp0_a")+iString);
			Parameters->Store(Folder/TEXT("cp0_b")+iString+TEXT(".bin"), HDim, 1, TEXT("cp0_b")+iString);
			Parameters->Store(Folder/TEXT("cp1_a")+iString+TEXT(".bin"), HDim, HDim, TEXT("cp1_a")+iString);
			Parameters->Store(Folder/TEXT("cp1_b")+iString+TEXT(".bin"), HDim, 1, TEXT("cp1_b")+iString);
			Parameters->Store(Folder/TEXT("cp2_a")+iString+TEXT(".bin"), YDim, HDim, TEXT("cp2_a")+iString);
			Parameters->Store(Folder/TEXT("cp2_b")+iString+TEXT(".bin"), YDim, 1, TEXT("cp2_b")+iString);
		}
	}

	void LoadParametersDerived() override
	{
		Xmean = CreateTensor(Parameters->Load("Xmean"));
		Xstd = CreateTensor(Parameters->Load("Xstd"));
		Ymean = CreateTensor(Parameters->Load("Ymean"));
		Ystd = CreateTensor(Parameters->Load("Ystd"));
		BW0 = CreateTensor(Parameters->Load("wc0_w"));
		Bb0 = CreateTensor(Parameters->Load("wc0_b"));
		BW1 = CreateTensor(Parameters->Load("wc1_w"));
		Bb1 = CreateTensor(Parameters->Load("wc1_b"));
		BW2 = CreateTensor(Parameters->Load("wc2_w"));
		Bb2 = CreateTensor(Parameters->Load("wc2_b"));
		CW.SetNumZeroed(6 * YDimBlend, true);
		for(int32 i = 0; i < YDimBlend; i++)
		{
			const FString iString = FString::FromInt(i);
			CW[6 * i + 0] = CreateTensor(Parameters->Load("cp0_a" + iString));
			CW[6 * i + 1] = CreateTensor(Parameters->Load("cp0_b" + iString));
			CW[6 * i + 2] = CreateTensor(Parameters->Load("cp1_a" + iString));
			CW[6 * i + 3] = CreateTensor(Parameters->Load("cp1_b" + iString));
			CW[6 * i + 4] = CreateTensor(Parameters->Load("cp2_a" + iString));
			CW[6 * i + 5] = CreateTensor(Parameters->Load("cp2_b" + iString));
		}
		X = CreateTensor(XDim, 1, "X");
		Y = CreateTensor(YDim, 1, "Y");
		BX = CreateTensor(XDimBlend, 1, "BX");
		BY = CreateTensor(YDimBlend, 1, "BY");
		W0 = CreateTensor(HDim, XDim, "W0");
		W1 = CreateTensor(HDim, HDim, "W1");
		W2 = CreateTensor(YDim, HDim, "W2");
		b0 = CreateTensor(HDim, 1, "b0");
		b1 = CreateTensor(HDim, 1, "b1");
		b2 = CreateTensor(YDim, 1, "b2");
	}

	void Predict() override
	{
		//Normalise Input
		Normalise(X, Xmean, Xstd, Y);

		//Process Gating Network
		for(int32 i = 0; i < ControlNeurons.Num(); i++)
		{
			BX->SetValue(i, 0, Y->GetValue(ControlNeurons[i], 0));
		}
		ELU(Layer(BX, BW0, Bb0, BY));
		ELU(Layer(BY, BW1, Bb1, BY));
		SoftMax(Layer(BY, BW2, Bb2, BY));

		//Generate Network Weights
		W0->SetZero();
		b0->SetZero();
		W1->SetZero();
		b1->SetZero();
		W2->SetZero();
		b2->SetZero();
		for(int32 i = 0; i < YDimBlend; i++)
		{
			const float weight = BY->GetValue(i, 0);
			Blend(W0, CW[6 * i + 0], weight);
			Blend(b0, CW[6 * i + 1], weight);
			Blend(W1, CW[6 * i + 2], weight);
			Blend(b1, CW[6 * i + 3], weight);
			Blend(W2, CW[6 * i + 4], weight);
			Blend(b2, CW[6 * i + 5], weight);
		}

		//Process Motion-Prediction Network
		ELU(Layer(Y, W0, b0, Y));
		ELU(Layer(Y, W1, b1, Y));
		Layer(Y, W2, b2, Y);

		//Renormalise Output
		Renormalise(Y, Ymean, Ystd, Y);
	}
};
