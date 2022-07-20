// juaxix - 2020-2022 - xixgames

#pragma once

#include "DL_NeuralNetworkComponent.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DeepLearningHelpersBPLibrary.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDL_OnAsyncDataLoadingCompleted);

UCLASS()
class DEEPLEARNINGHELPERS_API UDL_AsyncDataLoadAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	/** Creates a loading  async task */
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject", Category = "AI|Deep Learning|Wrappers"))
	static UDL_AsyncDataLoadAction* DeepLearningAsyncDataLoad(UDL_NeuralNetworkComponent* NeuralNetwork, UObject* WorldContextObject)
	{
		UDL_AsyncDataLoadAction* AsyncDataLoadAction = NewObject<UDL_AsyncDataLoadAction>(WorldContextObject);
		AsyncDataLoadAction->NeuralNetwork = NeuralNetwork;

		return AsyncDataLoadAction;
	}

	UPROPERTY(BlueprintAssignable)
	FDL_OnAsyncDataLoadingCompleted OnCompleted;

	UPROPERTY(Transient)
	UDL_NeuralNetworkComponent* NeuralNetwork = nullptr;
protected:
	// UBlueprintAsyncActionBase interface begin
	virtual void Activate() override
	{
		if (IsValid(NeuralNetwork))
		{
			AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [&]()->void
			{
				NeuralNetwork->LoadParameters();
				OnCompleted.Broadcast();
				SetReadyToDestroy();
			});
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NeuralNetwork is invalid for this UDL_AsyncDataLoadAction"));
			SetReadyToDestroy();
		}
	}
	//~UBlueprintAsyncActionBase interface end

};


UCLASS(BlueprintType)
class DEEPLEARNINGHELPERS_API UDeepLearningHelpersBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
};
