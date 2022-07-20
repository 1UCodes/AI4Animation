// juaxix - 2020-2022 - xixgames

#include "DL_BioAnimationCharacter.h"

// Engine includes
#include "ControlRigComponent.h"

// Deep Learning includes
#include "DL_ActorComponent.h"
#include "DL_MANN.h"
#include "DL_PlayerController.h"
#include "Utility/Utility.h"

ADL_BioAnimationCharacter::ADL_BioAnimationCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

	ControlRig = CreateDefaultSubobject<UControlRigComponent>(TEXT("ControlRig"));
	ControlRig->SetupAttachment(RootComponent);

	Actor = CreateDefaultSubobject<UDL_ActorComponent>(TEXT("Actor"));
	AddOwnedComponent(Actor);

	NN = CreateDefaultSubobject<UDL_MANN>(TEXT("MANN"));
	NN->SetupAttachment(RootComponent);
}

// Called to bind functionality to input
void ADL_BioAnimationCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// TODO : bind inputs - cache input component in the player controller
}

void ADL_BioAnimationCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GEngine)
	{
		GEngine->SetMaxFPS(60.0f);
	}
	// TODO: Load widget with same config as in unity:
	/*Forward = (Texture)Resources.Load("Forward");
	Left = (Texture)Resources.Load("Left");
	Right = (Texture)Resources.Load("Right");
	Back = (Texture)Resources.Load("Back");
	TurnLeft = (Texture)Resources.Load("TurnLeft");
	TurnRight = (Texture)Resources.Load("TurnRight");
	Disc = (Texture)Resources.Load("Disc");
	Idle = (Texture)Resources.Load("Idle");
	Move = (Texture)Resources.Load("Move");
	Jump = (Texture)Resources.Load("Jump");
	Sit = (Texture)Resources.Load("Sit");
	Lie = (Texture)Resources.Load("Lie");
	Stand = (Texture)Resources.Load("Stand");
	FontStyle = new GUIStyle();
	FontStyle.font = (Font)Resources.Load("Fonts/Coolvetica");
	FontStyle.normal.textColor = Color.white;
	FontStyle.alignment = TextAnchor.MiddleCenter;*/
}

void ADL_BioAnimationCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!NN->Parameters || !NN->X || !NN->Y) 
	{
		return;
	}

	if (!IsValid(CachedPlayerController))
	{
		return;
	}

	if (bTrajectoryControl) 
	{
		PredictTrajectory();
	}

	Animate();

	/** TODO
	if (MotionEditing != nullptr) 
	{
		MotionEditing->Process();
		for (int32 i=0; i < Actor->Bones.Num(); i++) {
			FVector Position = Actor->Bones[i].BoneLocation();
			Position.Z = Positions[i].Z;
			Positions[i] = FMath::Lerp(Positions[i], Position, MotionEditing->GetStability());
		}
	}*/
}

void ADL_BioAnimationCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	CachedPlayerController = Cast<ADL_PlayerController>(NewController);
	// In Unreal Y is Z. In Unity it's switched.
	const FVector& Forward = GetActorTransform().GetLocation().ForwardVector;
	TargetDirection = FVector(Forward.X, Forward.Y, 0.0f);
	Actor->ExtractSkeleton();
	const int32 NumBones = Actor->Bones.Num();
	Positions.SetNum(NumBones);
	Forwards.SetNum(NumBones);
	Ups.SetNum(NumBones);
	Velocities.SetNum(NumBones);
	Trajectory.Init(Points, CachedPlayerController->GetNames(), GetActorLocation(), TargetDirection);
	if (CachedPlayerController->Styles.Num() > 0)
	{
		for(int32 i = 0; i < Trajectory.Points.Num(); i++)
		{
			Trajectory.Points[i].Styles[0] = 1.0f;
		}
	}

	for (int32 i = 0; i < Actor->Bones.Num(); i++) 
	{
		const FTransform BoneTransform = Actor->Bones[i].BoneTransform();
		Positions[i] = BoneTransform.GetLocation();
		Forwards[i] = BoneTransform.GetRotation().GetForwardVector();
		Ups[i] = BoneTransform.GetRotation().GetUpVector();
		Velocities[i] = FVector::ZeroVector;
	}

	// parameters will be loaded async in blueprints: NN->LoadParameters();
}

void ADL_BioAnimationCharacter::Reinitialise()
{
	SetActorLocation(FVector::ZeroVector);
	TargetDirection = GetActorForwardVector();
	TargetVelocity = FVector::ZeroVector;
	PID = DeepLearningHelpers::PIDController{0.2f, 0.8f, 0.0f};
	const int32 NumBones = Actor->Bones.Num();
	Positions.Empty(NumBones);
	Forwards.Empty(NumBones);
	Ups.Empty(NumBones);
	Velocities.Empty(NumBones);
	Trajectory.Init(Points, CachedPlayerController->GetNames(), GetActorLocation(), TargetDirection);

	for (int32 i=0; i < Trajectory.Points.Num(); i++) 
	{
		Trajectory.Points[i].Styles[0] = 1.0f;
	}

	for (int32 i=0; i < NumBones; i++) 
	{
		FTransform BoneTransform = Actor->Bones[i].BoneTransform();
		Positions[i] = BoneTransform.GetLocation();
		Forwards[i] = BoneTransform.GetRotation().GetForwardVector();
		Ups[i] = BoneTransform.GetRotation().GetUpVector();
		Velocities[i] = FVector::ZeroVector;
	}
}

float ADL_BioAnimationCharacter::PoolBias()
{
	const TArray<float>& Styles = Trajectory.Points[RootPointIndex].Styles;
	float Bias = 0.0f;
	
	for (int32 i = 0; i < Styles.Num(); i++) 
	{
		float _bias = CachedPlayerController->Styles[i].Bias;
		float max = 0.0f;
		
		for (int32 j = 0; j < CachedPlayerController->Styles[i].Multipliers.Num(); j++) 
		{
			// TODO
			if (CachedPlayerController->Styles[i].Query() && false /*Input.GetKey(Controller.Styles[i].Multipliers[j].Key)*/) 
			{
				max = FMath::Max(max, CachedPlayerController->Styles[i].Bias * CachedPlayerController->Styles[i].Multipliers[j].Value);
			}
		}
		for (int32 j=0; j < CachedPlayerController->Styles[i].Multipliers.Num(); j++) 
		{
			// TODO
			if (CachedPlayerController->Styles[i].Query() && false /*Input.GetKey(Controller.Styles[i].Multipliers[j].Key)*/) 
			{
				_bias = FMath::Min(max, _bias * CachedPlayerController->Styles[i].Multipliers[j].Value);
			}
		}

		Bias += Styles[i] * _bias;
	}

	return Bias;
}

void ADL_BioAnimationCharacter::PredictTrajectory()
{
	//Query Control
	float Turn = CachedPlayerController->QueryTurn();
	FVector Move = CachedPlayerController->QueryMove();
	TArray<float> Style = CachedPlayerController->GetStyle();
	bool bControl = Turn != 0.0f || Move.Size() != 0.0f || Style[1] != 0.0f;

	//Intro Motion
	if (GetWorld()->TimeSeconds < 1.0f)
	{
		Move = FVector::ForwardVector;
		Turn = 0.0f;
		Style.SetNum(FMath::Max(Style.Num(), 4));
		Style[1] = 1.0f;
		bControl = true;
	}

	//Process Control
	const float Curvature = Trajectory.GetCurvature(0, 111, 10);
	const float Target = PoolBias();
	const float Current = Trajectory.Points[RootPointIndex].GetVelocity().Size();
	float Bias = Target;
	
	if (Turn == 0.0f) 
	{
		Bias += PID.Update(DeepLearningHelpers::Utility::Interpolate(Target, Current, Curvature), Current, 1.0f / Framerate);			
	} else {
		PID.Reset();
	}

	Move = Bias * Move.GetSafeNormal();

	if (Move.Size() == 0.0f && Turn != 0.0f) 
	{
		Move = 2.0f / 3.0f * FVector::ForwardVector;
	} else {
		// TODO
		if (Move.Y == 0.0f && Turn != 0.0f && false/*!Input.GetKey(Controller.Styles[1].Multipliers[1].Key)*/) 
		{
			Move = Bias * FVector(Move.X, 1.0f, 0.0f).GetSafeNormal();
		} else {
			Move = FMath::Lerp(Move, Bias * FVector::ForwardVector, Trajectory.Points[RootPointIndex].GetVelocity().Size() / 6.0f);
		}
	}

	if (Style[2] == 0.0f) 
	{
		Style[1] = FMath::Max(Style[1], FMath::Pow(FMath::Clamp(Trajectory.Points[RootPointIndex].GetVelocity().Size(), 0.0f, 1.0f), 2.0f));
		if(Style[1] > 0.0f) 
		{
			Move.Y = FMath::Max(Move.Y, 0.1f * Style[1]);
		}
	} else {
		Move.Y = Bias;
		Move.X = 0.0f;
		Turn = 0.0f;
		if (Curvature > 0.25f) 
		{
			Style[0] = 0.0f;
			Style[1] = 1.0f;
			Style[2] = 0.0f;
			Style[3] = 0.0f;
			Style[4] = 0.0f;
			Style[5] = 0.0f;
		} else {
			Style[0] = 0.0f;
			Style[1] = Trajectory.Points[RootPointIndex].GetVelocity().Size() < 0.5f ? 1.0f : 0.0f;
			Style[2] = Trajectory.Points[RootPointIndex].GetVelocity().Size() < 0.5f ? 0.0f : 1.0f;
			Style[3] = 0.0f;
			Style[4] = 0.0f;
			Style[5] = 0.0f;
		}
	}
	if (Style[3] > 0.0f || Style[4] > 0.0f || Style[5] > 0.0f) 
	{
		Bias = 0.0f;
		if (Trajectory.Points[RootPointIndex].GetVelocity().Size() > 0.5f) 
		{
			Style[3] = 0.0f;
			Style[4] = 0.0f;
			Style[5] = 0.0f;
		}
	}

	//Update Target Direction / Velocity / Correction
	TargetDirection = FMath::Lerp(TargetDirection, DeepLearningHelpers::Utility::AngleAxis(Turn * 60.0f, FVector::UpVector) * Trajectory.Points[RootPointIndex].GetDirection(), bControl ? TargetGain : TargetDecay);
	TargetVelocity = FMath::Lerp(TargetVelocity, TargetDirection * Move, bControl ? TargetGain : TargetDecay);
	TrajectoryCorrection = DeepLearningHelpers::Utility::Interpolate(TrajectoryCorrection, FMath::Max(Move.GetSafeNormal().Size(), FMath::Abs(Turn)), bControl ? TargetGain : TargetDecay);

	//Predict Future Trajectory
	TArray<FVector> trajectory_positions_blend;
	trajectory_positions_blend.SetNum(Trajectory.Points.Num());
	trajectory_positions_blend[RootPointIndex] = Trajectory.Points[RootPointIndex].GetPosition();

	for (int32 i = RootPointIndex + 1; i < Trajectory.Points.Num(); i++) 
	{
		const float Bias_pos = 0.75f;
		const float Bias_dir = 1.25f;
		const float Bias_vel = 1.0f;
		const float Weight = static_cast<float>(i - RootPointIndex) / static_cast<float>(FuturePoints); //w between 1/FuturePoints and 1
		const float Scale_pos = 1.0f - FMath::Pow(1.0f - Weight, Bias_pos);
		const float Scale_dir = 1.0f - FMath::Pow(1.0f - Weight, Bias_dir);
		const float Scale_vel = 1.0f - FMath::Pow(1.0f - Weight, Bias_vel);
		const float Scale = 1.0f / (Trajectory.Points.Num() - (RootPointIndex + 1.0f));
		trajectory_positions_blend[i] = trajectory_positions_blend[i-1] + FMath::Lerp(Trajectory.Points[i].GetPosition() - Trajectory.Points[i-1].GetPosition(), Scale * TargetVelocity, Scale_pos);
		Trajectory.Points[i].SetDirection(FMath::Lerp(Trajectory.Points[i].GetDirection(), TargetDirection, Scale_dir));
		Trajectory.Points[i].SetVelocity(FMath::Lerp(Trajectory.Points[i].GetVelocity(), TargetVelocity, Scale_vel));
	}

	for (int32 i = RootPointIndex + 1; i < Trajectory.Points.Num(); i++) 
	{
		Trajectory.Points[i].SetPosition(trajectory_positions_blend[i]);
	}

	for (int32 i = RootPointIndex; i < Trajectory.Points.Num(); i++)
	{
		const float Weight = static_cast<float>(i - RootPointIndex) / static_cast<float>(FuturePoints); //w between 0 and 1

		for(int32 j = 0; j < Trajectory.Points[i].Styles.Num(); j++) 
		{
			Trajectory.Points[i].Styles[j] = DeepLearningHelpers::Utility::Interpolate(Trajectory.Points[i].Styles[j], Style[j], DeepLearningHelpers::Utility::Normalise(Weight, 0.0f, 1.0f, CachedPlayerController->Styles[j].Transition, 1.0f));
		}
		
		DeepLearningHelpers::Utility::Normalise(Trajectory.Points[i].Styles);
		Trajectory.Points[i].SetSpeed(DeepLearningHelpers::Utility::Interpolate(Trajectory.Points[i].GetSpeed(), TargetVelocity.Size(), bControl ? TargetGain : TargetDecay));
	}
}

void ADL_BioAnimationCharacter::Animate()
{
	//Calculate Root
	FMatrix CurrentRoot = Trajectory.Points[RootPointIndex].GetTransformation();
	CurrentRoot.M[1][3] = 0.0f; //For flat terrain

	int32 Start = 0;

	//Input Trajectory Positions / Directions / Velocities / Styles
	for (int32 i = 0; i < PointSamples; i++) 
	{
		const FVector Pos = CurrentRoot.InverseTransformPosition(GetSample(i).GetPosition());
		const FVector Dir = CurrentRoot.InverseTransformVector(GetSample(i).GetDirection());
		const FVector Vel = CurrentRoot.InverseTransformVector(GetSample(i).GetVelocity());
		const float Speed = GetSample(i).GetSpeed();
		NN->SetInput(Start + i * TrajectoryDimIn + 0, Pos.X);
		NN->SetInput(Start + i * TrajectoryDimIn + 1, Pos.Y);
		NN->SetInput(Start + i * TrajectoryDimIn + 2, Dir.X);
		NN->SetInput(Start + i * TrajectoryDimIn + 3, Dir.Y);
		NN->SetInput(Start + i * TrajectoryDimIn + 4, Vel.X);
		NN->SetInput(Start + i * TrajectoryDimIn + 5, Vel.Y);
		NN->SetInput(Start + i * TrajectoryDimIn + 6, Speed);

		for(int32 j = 0; j < CachedPlayerController->Styles.Num(); j++) 
		{
			NN->SetInput(Start + i * TrajectoryDimIn + (TrajectoryDimIn - CachedPlayerController->Styles.Num()) + j, GetSample(i).Styles[j]);
		}
	}

	Start += TrajectoryDimIn*PointSamples;

	FMatrix PreviousRoot = Trajectory.Points[RootPointIndex-1].GetTransformation();
	PreviousRoot.M[1][3] = 0.0f; //For flat terrain

	//Input Previous Bone Positions / Velocities
	for (int32 i = 0; i < Actor->Bones.Num(); i++) 
	{
		const FVector Pos = PreviousRoot.InverseTransformPosition(Positions[i]);
		const FVector Forward = PreviousRoot.InverseTransformVector(Forwards[i]);
		const FVector Up = PreviousRoot.InverseTransformVector(Ups[i]);
		const FVector Vel = PreviousRoot.InverseTransformVector(Velocities[i]);
		NN->SetInput(Start + i * JointDimIn + 0, Pos.X);
		NN->SetInput(Start + i * JointDimIn + 1, Pos.Y);
		NN->SetInput(Start + i * JointDimIn + 2, Pos.Z);
		NN->SetInput(Start + i * JointDimIn + 3, Forward.X);
		NN->SetInput(Start + i * JointDimIn + 4, Forward.Y);
		NN->SetInput(Start + i * JointDimIn + 5, Forward.Z);
		NN->SetInput(Start + i * JointDimIn + 6, Up.X);
		NN->SetInput(Start + i * JointDimIn + 7, Up.Y);
		NN->SetInput(Start + i * JointDimIn + 8, Up.Z);
		NN->SetInput(Start + i * JointDimIn + 9, Vel.X);
		NN->SetInput(Start + i * JointDimIn + 10, Vel.Y);
		NN->SetInput(Start + i * JointDimIn + 11, Vel.Z);
	}

	Start += JointDimIn * Actor->Bones.Num();

	//Predict
	const double StartTime = FPlatformTime::Seconds();
	NN->Predict();
	NetworkPredictionTime = FPlatformTime::Seconds() - StartTime;

	//Update Past Trajectory
	for (int32 i = 0; i < RootPointIndex; i++)
	{
		Trajectory.Points[i].SetPosition(Trajectory.Points[i+1].GetPosition());
		Trajectory.Points[i].SetDirection(Trajectory.Points[i+1].GetDirection());
		Trajectory.Points[i].SetVelocity(Trajectory.Points[i+1].GetVelocity());
		Trajectory.Points[i].SetSpeed(Trajectory.Points[i+1].GetSpeed());
		for(int32 j = 0; j < Trajectory.Points[i].Styles.Num(); j++) 
		{
			Trajectory.Points[i].Styles[j] = Trajectory.Points[i+1].Styles[j];
		}
	}

	//Update Root
	float Update = FMath::Min(
		FMath::Pow(1.0f - (Trajectory.Points[RootPointIndex].Styles[0]), 0.25f),
		FMath::Pow(1.0f - (Trajectory.Points[RootPointIndex].Styles[3] 
						+ Trajectory.Points[RootPointIndex].Styles[4] 
						+ Trajectory.Points[RootPointIndex].Styles[5]
					), 	0.5f)
	);

	FVector RootMotion = Update * FVector(
		NN->GetOutput(TrajectoryDimOut * 6 + JointDimOut * Actor->Bones.Num() + 0), 
		NN->GetOutput(TrajectoryDimOut * 6 + JointDimOut * Actor->Bones.Num() + 1), 
		NN->GetOutput(TrajectoryDimOut * 6 + JointDimOut * Actor->Bones.Num() + 2));

	RootMotion /= Framerate;
	FVector Translation(RootMotion.X, RootMotion.Y, 0.0f); // unity Z axis is unreal Y axis
	float Angle = RootMotion.Z;

	Trajectory.Points[RootPointIndex].SetPosition(CurrentRoot.InverseTransformPosition(Translation));
	Trajectory.Points[RootPointIndex].SetDirection(DeepLearningHelpers::Utility::AngleAxis(Angle, FVector::UpVector) * Trajectory.Points[RootPointIndex].GetDirection());
	Trajectory.Points[RootPointIndex].SetVelocity(CurrentRoot.InverseTransformVector(Translation) * Framerate);
	FMatrix NextRoot = Trajectory.Points[RootPointIndex].GetTransformation();
	NextRoot.M[1][3] = 0.0f; //For flat terrain

	//Update Future Trajectory
	for(int32 i = RootPointIndex + 1; i < Trajectory.Points.Num(); i++)
	{
		Trajectory.Points[i].SetPosition(Trajectory.Points[i].GetPosition() + NextRoot.InverseTransformPosition(Translation));
		Trajectory.Points[i].SetDirection(DeepLearningHelpers::Utility::AngleAxis(Angle, FVector::UpVector) * Trajectory.Points[i].GetDirection());
		Trajectory.Points[i].SetVelocity(Trajectory.Points[i].GetVelocity() + NextRoot.InverseTransformVector(Translation) * Framerate);
	}

	Start = 0;

	for (int32 i = RootPointIndex + 1; i < Trajectory.Points.Num(); i++) 
	{
		//ROOT	1		2		3		4		5
		//.x....x.......x.......x.......x.......x
		int32 Index = i;
		int32 PrevSampleIndex = GetPreviousSample(Index).GetIndex() / PointDensity;
		int32 NextSampleIndex = GetNextSample(Index).GetIndex() / PointDensity;
		float Factor = static_cast<float>(i % PointDensity) / PointDensity;

		FVector PrevPos(
			NN->GetOutput(Start + (PrevSampleIndex-6)*TrajectoryDimOut + 0),
			NN->GetOutput(Start + (PrevSampleIndex-6)*TrajectoryDimOut + 1),
			0.0f // Y in unity
		);
		PrevPos = NextRoot.InverseTransformPosition(PrevPos);

		FVector PrevDir(
			NN->GetOutput(Start + (PrevSampleIndex-6)*TrajectoryDimOut + 2),
			NN->GetOutput(Start + (PrevSampleIndex-6)*TrajectoryDimOut + 3),
			0.0f // Y from Unity
		);
		PrevDir.Normalize();
		PrevDir = NextRoot.InverseTransformVector(PrevDir);

		FVector PrevVel(
			NN->GetOutput(Start + (PrevSampleIndex-6)*TrajectoryDimOut + 4),
			NN->GetOutput(Start + (PrevSampleIndex-6)*TrajectoryDimOut + 5),
			0.0f // Y from Unity
		);
		PrevVel = NextRoot.InverseTransformVector(PrevVel);

		FVector NextPos(
			NN->GetOutput(Start + (NextSampleIndex-6)*TrajectoryDimOut + 0),
			NN->GetOutput(Start + (NextSampleIndex-6)*TrajectoryDimOut + 1),
			0.0f // Y from Unity
		);
		NextPos = NextRoot.InverseTransformVector(NextPos);

		FVector NextDir(
			NN->GetOutput(Start + (NextSampleIndex-6)*TrajectoryDimOut + 2),
			NN->GetOutput(Start + (NextSampleIndex-6)*TrajectoryDimOut + 3),
			0.0f // Y from Unity
		);
		NextDir.Normalize();
		NextDir = NextRoot.InverseTransformVector(NextDir);

		FVector NextVel(
			NN->GetOutput(Start + (NextSampleIndex-6)*TrajectoryDimOut + 4),
			NN->GetOutput(Start + (NextSampleIndex-6)*TrajectoryDimOut + 5),
			0.0f // Y from Unity
		);
		NextVel = NextRoot.InverseTransformVector(NextVel);

		FVector Pos = (1.0f - Factor) * PrevPos + Factor * NextPos;
		FVector Dir = ((1.0f - Factor) * PrevDir + Factor * NextDir).GetSafeNormal();
		FVector Vel = (1.0f - Factor) * PrevVel + Factor * NextVel;

		Pos = FMath::Lerp(Trajectory.Points[i].GetPosition() + Vel / Framerate, Pos, 0.5f);

		Trajectory.Points[i].SetPosition(
			DeepLearningHelpers::Utility::Interpolate(
				Trajectory.Points[i].GetPosition(),
				Pos,
				TrajectoryCorrection
				)
			);
		Trajectory.Points[i].SetDirection(
			DeepLearningHelpers::Utility::Interpolate(
				Trajectory.Points[i].GetDirection(),
				Dir,
				TrajectoryCorrection
				)
			);
		Trajectory.Points[i].SetVelocity(
			DeepLearningHelpers::Utility::Interpolate(
				Trajectory.Points[i].GetVelocity(),
				Vel,
				TrajectoryCorrection
				)
			);
	}

	Start += TrajectoryDimOut * 6;

	//Compute Posture
	for (int32 i = 0; i < Actor->Bones.Num(); i++) 
	{
		FVector Position(NN->GetOutput(Start + i * JointDimOut + 0), NN->GetOutput(Start + i * JointDimOut + 1), NN->GetOutput(Start + i*JointDimOut + 2));
		Position = CurrentRoot.InverseTransformPosition(Position);
		FVector Forward(NN->GetOutput(Start + i * JointDimOut + 3), NN->GetOutput(Start + i * JointDimOut + 4), NN->GetOutput(Start + i*JointDimOut + 5));
		Forward.Normalize();
		Forward = CurrentRoot.InverseTransformVector(Forward);
		FVector Up(NN->GetOutput(Start + i*JointDimOut + 6), NN->GetOutput(Start + i * JointDimOut + 7), NN->GetOutput(Start + i*JointDimOut + 8));
		Up.Normalize();
		Up = CurrentRoot.InverseTransformVector(Up);
		FVector Velocity(NN->GetOutput(Start + i*JointDimOut + 9), NN->GetOutput(Start +  i * JointDimOut + 10), NN->GetOutput(Start + i*JointDimOut + 11));
		Velocity = CurrentRoot.InverseTransformVector(Velocity);

		Positions[i] = FMath::Lerp(Positions[i] + Velocity / Framerate, Position, 0.5f);
		Forwards[i] = Forward;
		Ups[i] = Up;
		Velocities[i] = Velocity;
	}

	Start += JointDimOut * Actor->Bones.Num();
	
	//Assign Posture
	//SetActorLocation(DeepLearningHelpers::Utility::GetPositionFromMatrix(NextRoot)); // TODO : use movement component
	//SetActorRotation(DeepLearningHelpers::Utility::GetRotationFromMatrix(NextRoot));
	// TODO: use the control rig plugin
	//for (int32 i = 0; i < Actor->Bones.Num(); i++) 
	//{
		//Actor->Bones[i].SetBoneLocation(Positions[i]);
		//Actor->Bones[i].SetBoneRotation(FRotationMatrix::MakeFromX(Ups[i] - Forwards[i])); // Look rotation
	//}
}
