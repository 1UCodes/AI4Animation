// juaxix - 2020-2022 - xixgames

#include "DL_ActorComponent.h"

#include "DrawDebugHelpers.h"
#include "MeshUtilities.h"
#include "SkeletalRenderPublic.h"

FDL_Bone* FDL_Bone::GetParent() const
{
	return ParentIndex == -1 ? nullptr : &ActorComponent->Bones[ParentIndex];
}

FDL_Bone* FDL_Bone::GetChild(int32 InIndex) const
{
	return !Childs.IsValidIndex(InIndex) ? nullptr : &ActorComponent->Bones[Childs[InIndex]];
}

FVector FDL_Bone::GetBoneLocationByIndex(int32 InIndex) const
{
	return	InIndex > -1 
			? ActorComponent->SkeletalMeshComponent->GetBoneTransform(InIndex).GetLocation()
			: FVector::ZeroVector;
}

float FDL_Bone::GetLength() const
{
	if (GetParent() == nullptr)
	{
		return 0.0f;
	}

	return FVector::Dist(BoneLocation(), ParentBoneLocation());
}

UDL_ActorComponent::UDL_ActorComponent()
	: bInspectSkeleton(false)
	, bDrawRoot(false)
	, bDrawSkeleton(false)
	, bDrawVelocities(false)
	, bDrawTransforms(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDL_ActorComponent::BeginPlay()
{
	Super::BeginPlay();
	ExtractSkeleton();
}

void UDL_ActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	DebugDraw();
}

void UDL_ActorComponent::AddSkeletonNodes()
{
	const int32 NumBones = SkeletalMeshComponent->GetNumBones();
	const USkeleton* Skeleton = SkeletalMeshComponent->SkeletalMesh->GetSkeleton();
	const FReferenceSkeleton& RefSkeleton = SkeletalMeshComponent->SkeletalMesh->GetRefSkeleton();
	for (int32 BoneIndex = 0; BoneIndex < NumBones; ++BoneIndex)
	{
		TArray<int32> BoneChilds;
		Skeleton->GetChildBones(BoneIndex, BoneChilds);
		Bones.Emplace(FVector::ZeroVector, BoneIndex, RefSkeleton.GetParentIndex(BoneIndex), BoneChilds, RefSkeleton.GetBoneName(BoneIndex), this);
	}
}

void UDL_ActorComponent::DebugDrawBoneRecursive(const FDL_Bone& Bone) const
{
	const UWorld* const World = GetWorld();
	const FVector ParentBoneLocation = Bone.ParentBoneLocation();
	if (Bone.Index > -1 && Bone.ParentIndex != Bone.Index && ParentBoneLocation != FVector::ZeroVector)
	{
		const FVector BoneLocation = Bone.BoneLocation();
		DrawDebugDirectionalArrow(World, BoneLocation, ParentBoneLocation, BoneSize, BoneColor, false, 0.0f, SDPG_Foreground, 2.1f);
	}

	for (const int32 BoneIndex : Bone.Childs)
	{
		if (const FDL_Bone* BoneChild = Bone.GetChild(BoneIndex))
		{
			DebugDrawBoneRecursive(*BoneChild);
		}
	}
}

void UDL_ActorComponent::DebugDraw() const
{
	if (bDrawSkeleton && Bones.Num() > 0)
	{
		for (const auto& Bone : Bones)
		{
			DebugDrawBoneRecursive(Bone);
		}
	}
}

void UDL_ActorComponent::ExtractSkeleton()
{
	if (!IsValid(SkeletalMeshComponent))
	{
		SkeletalMeshComponent = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		if (!SkeletalMeshComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("Skeletal Mesh Component is null"));
			return;
		}
	}

	if (SkeletalMeshComponent->GetNumBones() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Skeletal Mesh: Bones number is 0"));
		return;
	}

	Bones.Empty();
	AddSkeletonNodes();
}
