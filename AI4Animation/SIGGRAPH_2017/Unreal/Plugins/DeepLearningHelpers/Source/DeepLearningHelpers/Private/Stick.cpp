// juaxix - 2020 - xixgames


#include "Stick.h"

// Sets default values for this component's properties
UStick::UStick()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStick::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UStick::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(Target == nullptr)
	{
		return;
	}
	FVector position = GetOwner()->GetActorLocation();
	FVector target = Target->GetActorLocation();
	if(X)
	{
		position.X = target.X;
	}
	if(Y)
	{
		position.Y = target.Y;
	}
	if(Z)
	{
		position.Z = target.Z;
	}
	GetOwner()->SetActorLocation(position);
}
