// juaxix - 2020-2022 - xixgames

#pragma once

#include "CoreMinimal.h"
#include "Animation/Trajectory.h"
#include "GameFramework/Character.h"
#include "Tools/PIDController.h"
#include "DL_BioAnimationCharacter.generated.h"

class UDL_MANN;
class UDL_ActorComponent;
class ADL_PlayerController;
class UControlRigComponent;

UCLASS()
class DEEPLEARNINGHELPERS_API ADL_BioAnimationCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Setup")
	bool bInspect = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Setup")
	bool bShowTrajectory = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Setup")
	bool bShowVelocities = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Setup")
	float TargetGain = 0.25f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Setup")
	float TargetDecay = 0.05f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Setup")
	bool bTrajectoryControl = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Setup")
	float TrajectoryCorrection = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Setup")
	UControlRigComponent* ControlRig = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Setup")
	ADL_PlayerController* CachedPlayerController = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Setup")
	UDL_ActorComponent* Actor = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Setup")
	UDL_MANN* NN = nullptr;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Trajectory")
	FVector TargetDirection = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "Trajectory")
	FVector TargetVelocity = FVector::ZeroVector;

	DeepLearningHelpers::FDL_Trajectory Trajectory;
	DeepLearningHelpers::PIDController PID{0.2f, 0.8f, 0.0f};

	//State
	TArray<FVector> Positions;
	TArray<FVector> Forwards;
	TArray<FVector> Ups;
	TArray<FVector> Velocities;

	//NN Parameters
	const int32 TrajectoryDimIn = 13;
	const int32 TrajectoryDimOut = 6;
	const int32 JointDimIn = 12;
	const int32 JointDimOut = 12;

	//Trajectory for 60 Hz framerate
	const int32 Framerate = 60;
	const int32 Points = 111;
	const int32 PointSamples = 12;
	const int32 PastPoints = 60;
	const int32 FuturePoints = 50;
	const int32 RootPointIndex = 60;
	const int32 PointDensity = 10;

	//Post-Processing
	// TODO
	//private MotionEditing MotionEditing;
	
	//GUI : TODO
	// Texture2D Forward, Left, Right, Back, TurnLeft, TurnRight, Disc;
	// Texture2D Idle, Move, Jump, Sit, Lie, Stand;
	// UFontStyle FontStyle;
	
	//Performance
	double NetworkPredictionTime;

	ADL_BioAnimationCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void Reinitialise();

private:
	virtual float PoolBias();

	virtual void PredictTrajectory();

	virtual void Animate();

	DeepLearningHelpers::FDL_Trajectory::Point GetSample(int32 Index) const
	{
		return Trajectory.Points[FMath::Clamp(Index * 10, 0, Trajectory.Points.Num() - 1)];
	}

	DeepLearningHelpers::FDL_Trajectory::Point GetPreviousSample(int32 Index) const
	{
		return GetSample(Index / 10);
	}

	DeepLearningHelpers::FDL_Trajectory::Point GetNextSample(int32 Index) const
	{
		if (Index % 10 == 0) 
		{
			return GetSample(Index / 10);
		}

		return GetSample(Index / 10 + 1);
	}
};
