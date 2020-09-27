// juaxix - 2020 - xixgames

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Stick.generated.h"

/**
 * Component to make owner actor to follow another actor transform position
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DEEPLEARNINGHELPERS_API UStick : public USceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStick();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Gameplay")
	AActor* Target = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Gameplay")
	bool X;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Gameplay")
	bool Y;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Gameplay")
	bool Z;
};
