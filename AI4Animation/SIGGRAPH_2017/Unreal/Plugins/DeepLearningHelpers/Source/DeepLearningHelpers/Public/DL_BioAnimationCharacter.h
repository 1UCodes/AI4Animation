// juaxix - 2020-2022 - xixgames

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Tools/PIDController.h"
#include "DL_BioAnimationCharacter.generated.h"

class UDL_MANN;
class ADL_Controller;
class UDL_ActorComponent;

UCLASS()
class DEEPLEARNINGHELPERS_API ADL_BioAnimationCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	bool bInspect = false;

	bool bShowTrajectory = true;
	bool bShowVelocities = true;

	float TargetGain = 0.25f;
	float TargetDecay = 0.05f;
	bool bTrajectoryControl = true;
	float TrajectoryCorrection = 1.0f;

	//UPROPERTY(Transient)
	//ADL_Controller* Controller = nullptr;

	UPROPERTY(Transient)
	UDL_ActorComponent* Actor = nullptr;

	UPROPERTY(Transient)
	UDL_MANN* NN = nullptr;

	// TODO Trajectory Trajectory;

	FVector TargetDirection;
	FVector TargetVelocity;
	DeepLearningHelpers::PIDController PID;

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
	float NetworkPredictionTime;

	ADL_BioAnimationCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
