// juaxix - 2020-2022 - xixgames

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "GameFramework/PlayerController.h"
#include "DL_PlayerController.generated.h"

USTRUCT(BlueprintType)
struct FDLPlayerInputMultiplier
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	FKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Player Input")
	float Value;
};

USTRUCT(BlueprintType)
struct FDLPlayerInputKey
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	FKey Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	bool bNegate = false;
};

USTRUCT(BlueprintType)
struct FDLPlayerInputStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	float Bias = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	float Transition = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	TArray<FDLPlayerInputKey> Keys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	TArray<FDLPlayerInputMultiplier> Multipliers;
	
	bool Query() const;

	void SetKeyCount(int32 InCount);
};


UCLASS(BlueprintType)
class DEEPLEARNINGHELPERS_API ADL_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	bool bInspect = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Input")
	TArray<FDLPlayerInputStyle> Styles;

	const FName Forward{"MoveForward"};
	const FName Back{"MoveBackward"};
	const FName Left{"MoveLeft"};
	const FName Right{"MoveRight"};
	const FName TurnLeft{"TurnLeft"};
	const FName TurnRight{"TurnRight"};

	ADL_PlayerController();

	virtual void SetupInputComponent() override;

	TArray<float> GetStyle() const;

	TArray<FString> GetNames() const;

	FVector QueryMove() const;

	float QueryTurn() const;

	void SetStyleCount(int32 Count)
	{
		Count = FMath::Max(Count, 0);
		if (Styles.Num() != Count) 
		{
			Styles.SetNum(Count);
		}
	}

	bool QueryAny() const;

	float PoolBias(const TArray<float>& Weights) const;

protected:
	void ToggleInGameMenu();
};
