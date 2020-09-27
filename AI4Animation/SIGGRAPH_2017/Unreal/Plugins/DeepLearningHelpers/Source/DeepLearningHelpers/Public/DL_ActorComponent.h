// juaxix - 2020 - xixgames

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DL_ActorComponent.generated.h"


class UDL_ActorComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) , Category = "AI|Deep Learning|Bones")
class DEEPLEARNINGHELPERS_API UDL_Bone: public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Bones")
	FVector Velocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Bones")
	int32 Index;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Bones")
	int32 ParentIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Bones")
	TArray<int32> Childs;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Bones")
	FName BoneName;

	UDL_ActorComponent* Actor;

	UDL_Bone()
	{}

	UFUNCTION(BlueprintCallable, Category = "AI|Deep Learning|Bones")
	void Init(UDL_ActorComponent* InActor, const FName& InBoneName, int32 InBoneIndex)
	{
		Actor = InActor;
		BoneName = InBoneName;
		Velocity = FVector::ZeroVector;
		Index = InBoneIndex;
		ParentIndex = -1;
	}

	/*FString GetName()
	{
		return UGameplayStatics::  Transform;
	}*/

	UDL_Bone* GetParent();
	UDL_Bone* GetChild(int32 InIndex);
	FVector GetBoneLocationByIndex(int32 InIndex) const;
	FVector BoneLocation() const{return GetBoneLocationByIndex( Index );}
	FVector ParentBoneLocation() const{return GetBoneLocationByIndex( ParentIndex );}
	float GetLength();
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) , Category = "AI|Deep Learning|Actor")
class DEEPLEARNINGHELPERS_API UDL_ActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	bool InspectSkeleton = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	bool DrawRoot = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	bool DrawSkeleton = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	bool DrawVelocities = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	bool DrawTransforms = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	float BoneSize = 0.025f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	FColor BoneColor = FColor::Black;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	FColor JointColor = FColor::Yellow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	TArray<UDL_Bone*> Bones;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	USkeletalMeshComponent* SkeletalMeshComponent;

	UDL_ActorComponent();

protected:
	virtual void BeginPlay() override;

	/// <summary>
	/// Recursive method to extract bones into the bones array
	/// </summary>
	/// <param name="InBoneIndex"></param>
	/// <param name="InParentBoneIndex"></param>
	virtual void AddSkeletonNodeRecursive(int32 InBoneIndex, int32 InParentBoneIndex = -1);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// Builds the Bone Tree
	/// </summary>
	virtual void ExtractSkeleton();
};
