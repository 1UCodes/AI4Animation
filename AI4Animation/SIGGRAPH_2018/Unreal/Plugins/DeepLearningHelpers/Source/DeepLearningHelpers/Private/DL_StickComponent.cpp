// juaxix - 2020-2022 - xixgames


#include "DL_StickComponent.h"

UDL_StickComponent::UDL_StickComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}



// Called every frame
void UDL_StickComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Target == nullptr)
	{
		return;
	}
	FVector Position = GetOwner()->GetActorLocation();
	const FVector TargetPosition = Target->GetActorLocation();
	if (bX)
	{
		Position.X = TargetPosition.X;
	}

	if (bY)
	{
		Position.Y = TargetPosition.Y;
	}

	if (bZ)
	{
		Position.Z = TargetPosition.Z;
	}

	GetOwner()->SetActorLocation(Position);
}
