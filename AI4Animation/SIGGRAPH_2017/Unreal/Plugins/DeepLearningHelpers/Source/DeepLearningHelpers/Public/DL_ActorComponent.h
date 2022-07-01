// juaxix - 2020-2022 - xixgames

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DL_ActorComponent.generated.h"


class UDL_ActorComponent;

USTRUCT( Blueprintable, Category = "AI|Deep Learning|Bones")
struct DEEPLEARNINGHELPERS_API FDL_Bone
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Bones")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Bones")
	int32 Index = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Bones")
	int32 ParentIndex = -1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Bones")
	TArray<int32> Childs;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Bones")
	FName BoneName = TEXT("");

	UPROPERTY(Transient)
	UDL_ActorComponent* ActorComponent = nullptr;

	FDL_Bone() = default;

	explicit FDL_Bone(const FVector& InVelocity, const int32 InIndex, const int32 InParentIndex, TArray<int32>& InChilds, const FName& InBoneName, UDL_ActorComponent* InActorComponent)
	{
		Velocity = InVelocity;
		Index = InIndex;
		ParentIndex = InParentIndex;
		Childs = MoveTemp(InChilds);
		BoneName = InBoneName;
		ActorComponent = InActorComponent;
	}

	FDL_Bone* GetParent() const;
	FDL_Bone* GetChild(int32 InIndex) const;
	FVector GetBoneLocationByIndex(int32 InIndex) const;
	FVector BoneLocation() const {return GetBoneLocationByIndex(Index);}
	FVector ParentBoneLocation() const {return GetBoneLocationByIndex(ParentIndex);}
	float GetLength() const;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) , Category = "AI|Deep Learning|Actor")
class DEEPLEARNINGHELPERS_API UDL_ActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	uint8 bInspectSkeleton:1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	uint8 bDrawRoot:1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	uint8 bDrawSkeleton:1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	uint8 bDrawVelocities:1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	uint8 bDrawTransforms:1;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	float BoneSize = 0.025f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	float BoneWidth = 12.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	FColor BoneColor = FColor::Black;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	FColor JointColor = FColor::Yellow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Actor")
	TArray<FDL_Bone> Bones;

	UPROPERTY(Transient)
	USkeletalMeshComponent* SkeletalMeshComponent = nullptr;

	UDL_ActorComponent();

protected:
	virtual void BeginPlay() override;

	virtual void AddSkeletonNodes();

	void DebugDrawBoneRecursive(const FDL_Bone& Bone) const;

	virtual void DebugDraw() const;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/// <summary>
	/// Builds the Bone Tree
	/// </summary>
	virtual void ExtractSkeleton();
};
