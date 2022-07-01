// juaxix - 2020-2022 - xixgames

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "DL_StickComponent.generated.h"

/**
 * Component to make owner actor to follow another actor transform position
 */
UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPLEARNINGHELPERS_API UDL_StickComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UDL_StickComponent();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Gameplay")
	AActor* Target = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Gameplay")
	bool bX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Gameplay")
	bool bY;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Gameplay")
	bool bZ;
};
