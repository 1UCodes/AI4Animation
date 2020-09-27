// juaxix - 2020 - xixgames

#include "DL_ActorComponent.h"

#include "MeshUtilities.h"
#include "SkeletalRenderPublic.h"

UDL_Bone* UDL_Bone::GetParent()
{
	return ParentIndex == -1 ? nullptr : Actor->Bones[ParentIndex];
}

UDL_Bone* UDL_Bone::GetChild(int32 InIndex)
{
	return InIndex >= Childs.Num() ? nullptr : Actor->Bones[Childs[InIndex]];
}

FVector UDL_Bone::GetBoneLocationByIndex(int32 InIndex) const
{
	if ( InIndex == -1 )
	{
		return FVector::ZeroVector;
	}

	return Actor->SkeletalMeshComponent->GetBoneTransform(
		InIndex,
		Actor->GetOwner()->GetActorTransform()
	).GetLocation();
}

float UDL_Bone::GetLength()
{
	if(GetParent() == nullptr)
	{
		return 0.0f;
	}

	return FVector::Dist(BoneLocation(), ParentBoneLocation());
}

UDL_ActorComponent::UDL_ActorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDL_ActorComponent::BeginPlay()
{
	Super::BeginPlay();
	ExtractSkeleton();
}


void UDL_ActorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
													FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDL_ActorComponent::AddSkeletonNodeRecursive(int32 InBoneIndex, int32 InParentBoneIndex)
{
	if (InBoneIndex==-1)
	{
		return;
	}

	// Create the Deep Learning bone
	FName BoneName = SkeletalMeshComponent->GetBoneName(InBoneIndex);
	UDL_Bone* bone = NewObject<UDL_Bone>(
	  this,
	  UDL_Bone::StaticClass(),
	  FName("Bone_" + BoneName.ToString() + "_" + FString::FromInt(InBoneIndex))
	  );
	bone->Init(this, BoneName, InBoneIndex);
	bone->ParentIndex = InParentBoneIndex;
	SkeletalMeshComponent->SkeletalMesh->Skeleton->GetChildBones(InBoneIndex, bone->Childs);
	Bones.Add(bone);

	// Scan the skeleton in a recursive way
	const int32 ChildrenNum = bone->Childs.Num();
	for (int32 i = 0; i < ChildrenNum; i++)
	{
		AddSkeletonNodeRecursive(bone->Childs[i], InBoneIndex);
	}
}

void UDL_ActorComponent::ExtractSkeleton()
{
	if (!SkeletalMeshComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Skeletal Mesh Component is null"));
		return;
	}

	if (SkeletalMeshComponent->GetNumBones() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Skeletal Mesh: Bones number is 0"));
		return;
	}

	Bones.Empty();

	AddSkeletonNodeRecursive(0);
}
