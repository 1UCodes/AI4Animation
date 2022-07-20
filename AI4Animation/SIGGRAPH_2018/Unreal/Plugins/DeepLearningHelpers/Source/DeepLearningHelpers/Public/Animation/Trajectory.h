// juaxix - 2020-2022 - xixgames

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetMathLibrary.h"

namespace DeepLearningHelpers
{
	struct DEEPLEARNINGHELPERS_API FDL_Trajectory
	{
	public:
		struct Point
		{
			int32 Index = -1;
			FMatrix Transformation;
			FVector Velocity;
			float Speed = 0.0f;
			FVector LeftSample;
			FVector RightSample;
			float Slope;
			float Phase;
			TArray<float> Signals;
			TArray<float> Styles;
			TArray<float> StyleUpdate;
			Point() = default;
			explicit Point(int InIndex, int InStyles)
			{
				Index = InIndex;
				Speed = 0.0f;
				Phase = 0.0f;
				Transformation = FMatrix::Identity;
				Velocity = FVector::ZeroVector;
				LeftSample = FVector::ZeroVector;
				RightSample = FVector::ZeroVector;
				Slope = 0.f;
				Signals.SetNum(InStyles);
				Styles.SetNum(InStyles);
				StyleUpdate.SetNum(InStyles);
			}

			void SetIndex(int32 NewIndex) 
			{
				Index = NewIndex;
			}

			int32 GetIndex() const
			{
				return Index;
			}

			void SetTransformation(const FMatrix& Matrix) 
			{
				Transformation = Matrix;
			}

			FMatrix& GetTransformation() 
			{
				return Transformation;
			}

			void SetPosition(const FVector& Position) 
			{
				Transformation.M[0][3] = Position.X;
				Transformation.M[1][3] = Position.Y;
				Transformation.M[2][3] = Position.Z;
			}

			FVector GetPosition() const
			{
				return FVector(Transformation.M[0][3], Transformation.M[1][3], Transformation.M[2][3]);
			}

			void SetRotation(const FQuat& Rotation) 
			{
				const FVector Right = Rotation.GetRightVector();
				const FVector Up = Rotation.GetUpVector();
				const FVector Forward = Rotation.GetForwardVector();
				Transformation.M[0][0] = Right.X;
				Transformation.M[1][0] = Right.Y;
				Transformation.M[2][0] = Right.Z;
				Transformation.M[0][1] = Up.X;
				Transformation.M[1][1] = Up.Y;
				Transformation.M[2][1] = Up.Z;
				Transformation.M[0][2] = Forward.X;
				Transformation.M[1][2] = Forward.Y;
				Transformation.M[2][2] = Forward.Z;
			}

			FQuat GetRotation() const
			{
				return Transformation.ToQuat();
			}

			void SetDirection(FVector Direction) 
			{
				SetRotation(UKismetMathLibrary::FindLookAtRotation(
					Direction == FVector::ZeroVector ? FVector::ForwardVector : Direction, FVector::UpVector
					).Quaternion());
				 
			}

			FVector GetDirection() const
			{
				return Transformation.ToQuat().GetForwardVector();
			}

			void SetVelocity(FVector velocity)
			{
				Velocity = velocity;
			}

			FVector GetVelocity() 
			{
				return Velocity;
			}

			void SetSpeed(float speed) 
			{
				Speed = speed;
			}

			float GetSpeed() 
			{
				return Speed;
			}
			
			void SetPhase(float value) 
			{
				Phase = value;
			}

			float GetPhase()
			{
				return Phase;
			}

			void SetLeftsample(FVector position) 
			{
				LeftSample = position;
			}

			FVector GetLeftSample()
			{
				return LeftSample;
			}

			void SetRightSample(FVector position)
			{
				RightSample = position;
			}

			FVector GetRightSample()
			{
				return RightSample;
			}

			void SetSlope(float slope) 
			{
				Slope = slope;
			}

			float GetSlope() 
			{
				return Slope;
			}

			void Postprocess()
			{
				/*//LayerMask mask = LayerMask.GetMask("Ground");
				FVector position = Transformation.GetPosition();
				FVector direction = Transformation.GetForward();

				position.y = Utility.GetHeight(Transformation.GetPosition(), mask);
				SetPosition(position);

				Slope = Utility.GetSlope(position, mask);

				FVector ortho = Quaternion.Euler(0f, 90f, 0f) * direction;
				RightSample = position + Trajectory.Width * ortho.normalized;
				RightSample.y = Utility.GetHeight(RightSample, mask);
				LeftSample = position - Trajectory.Width * ortho.normalized;
				LeftSample.y = Utility.GetHeight(LeftSample, mask);*/
			}
		};

		bool bInspect = false;
		TArray<Point> Points;
		TArray<FString> Styles;

		static float Width;

		FDL_Trajectory() = default;

		explicit FDL_Trajectory(int32 InSize, const TArray<FString>& InStyles) 
		{
			bInspect = false;
			Points.Reserve(InSize);
			Styles = InStyles;
			for(int32 i=0; i < InSize; i++) 
			{
				Points.EmplaceAt(i, Point{i, Styles.Num()});
				Points[i].SetTransformation(FMatrix::Identity);
			}
		}

		explicit FDL_Trajectory(int32 InSize, const TArray<FString>& InStyles, const FVector& seedPosition, const FVector& seedDirection)
		{
			bInspect = false;
			Points.Reserve(InSize);
			Styles = InStyles;
			for(int32 i = 0; i < InSize; i++) 
			{
				Points.EmplaceAt(i, Point{i, Styles.Num()});
				Points[i].SetTransformation(MatrixTRS(seedPosition, UKismetMathLibrary::FindLookAtRotation(seedDirection, FVector::UpVector).Quaternion(), FVector::OneVector));
			}
		}

		explicit FDL_Trajectory(int32 InSize, const TArray<FString>& InStyles, const TArray<FVector>& positions, const TArray<FVector>& directions) 
		{
			bInspect = false;
			Points.Reserve(InSize);
			Styles = InStyles;
			for(int32 i=0; i < InSize; i++)
			{
				Points.EmplaceAt(i, Point{i, Styles.Num()});
				Points[i].SetTransformation(MatrixTRS(positions[i], UKismetMathLibrary::FindLookAtRotation(directions[i], FVector::UpVector).Quaternion(), FVector::OneVector));
			}
		}

		static FMatrix FromRotation(const FQuat& Q)
		{
			//rotation should be unit quaternion
			FMatrix Result;
			const float x = 2.0f * Q.X;
			const float y = 2.0f * Q.Y;
			const float z = 2.0f * Q.Z;
			const float qxx = Q.X * x;
			const float qyy = Q.Y * y;
			const float qzz = Q.Z * z;
			const float qxy = Q.X * y;
			const float qxz = Q.X * z;
			const float qyz = Q.Y * z;
			const float qwx = Q.W * x;
			const float qwy = Q.W * y;
			const float qwz = Q.W * z;
		
			Result.M[0][0] = 1.f - (qyy + qzz);
			Result.M[1][0] = qxy + qwz;
			Result.M[2][0] = qxz - qwy;
		
			Result.M[0][1] = qxy - qwz;
			Result.M[1][1] = 1.f - (qxx + qzz);
			Result.M[2][1] = qyz + qwx;
		
			Result.M[0][2] = qxz + qwy;
			Result.M[1][2] = qyz - qwx;
			Result.M[2][2] = 1.f - (qxx + qyy);
			
			return Result;
		}
		
		static FMatrix MatrixTRS(const FVector& Pos, const FQuat& Quat, const FVector& Scale)
		{
			FMatrix Result = FromRotation(Quat);

			Result.M[0][3] = Pos.X;
			Result.M[1][3] = Pos.Y;
			Result.M[2][3] = Pos.Z;
			Result.M[0][0] *= Scale.X;
			Result.M[0][1] *= Scale.Y;
			Result.M[0][2] *= Scale.Z;
			Result.M[1][0] *= Scale.X;
			Result.M[1][1] *= Scale.Y;
			Result.M[1][2] *= Scale.Z;
			Result.M[2][0] *= Scale.X;
			Result.M[2][1] *= Scale.Y;
			Result.M[2][2] *= Scale.Z;
			Result.M[3][3] = 1.f;
	
			return Result;
		}
		
		Point& GetFirst() 
		{
			return Points[0];
		}

		Point& GetLast() 
		{
			return Points[Points.Num()-1];
		}

		float GetLength() const
		{
			float length = 0.0f;
			for(int32 i = 1; i < Points.Num(); i++) 
			{
				length += FVector::Distance(Points[i-1].GetPosition(), Points[i].GetPosition());
			}

			return length;
		}

		float GetLength(int32 start, int32 end, int32 step) const
		{
			float length = 0.0f;
			for(int32 i = 0; i < end - step; i += step) 
			{
				length += FVector::Distance(Points[i+step].GetPosition(), Points[i].GetPosition());
			}

			return length;
		}

		void Init(int32 InSize, const TArray<FString>& InStyles, const FVector& InSeedPosition, const FVector& InSeedDirection)
		{
			bInspect = false;
			Points.Reserve(InSize);
			Styles = InStyles;
			for (int32 i = 0; i < InSize; i++)
			{
				Points.EmplaceAt(i, Point(i, Styles.Num()));
				Points[i].SetTransformation(MatrixTRS(InSeedPosition, InSeedDirection.ToOrientationQuat(), FVector::OneVector));
			}
		}

		static float SignedAngle(const FVector& V1, const FVector& V2)
		{
			const float DotVal = FVector::DotProduct(V1, V2);
			const float ClampedDot = (DotVal < -1.0f) ? -1.0f : ((DotVal > 1.0f) ? 1.0f : DotVal);
			const FVector Direction = FVector::CrossProduct(V1, V2);
			if ((Direction * Direction).IsNearlyZero())
			{
				return (DotVal < 0) ? PI : 0.0f;
			}
			
			const float Sign = Direction.Size() < 0.0f ? -1.0f : 1.0f;
			
			return Sign * UKismetMathLibrary::Acos(ClampedDot);
			
		}
		
		float GetCurvature(int32 start, int32 end, int32 step) const
		{
			float curvature = 0.0f;
			for(int32 i = step; i < end-step; i += step)
			{
				curvature += SignedAngle(Points[i].GetPosition() - Points[i-step].GetPosition(), Points[i+step].GetPosition() - Points[i].GetPosition() /*,FVector::UpVector*/);
			}

			curvature = UKismetMathLibrary::Abs(curvature);
			curvature = UKismetMathLibrary::Clamp(curvature / 180.0f, 0.0f, 1.0f);

			return curvature;
		}

		void Postprocess() 
		{
			for(int32 i=0; i<Points.Num(); i++) 
			{
				//Points[i].Postprocess();
			}
		}

		void Draw(int step=1)
		{
			TArray<FColor> colors = {}; //UltiDraw.GetRainbowColors(Styles.Length);

			//Connections
			for(int32 i=0; i<Points.Num() - step; i+=step) 
			{
				//UltiDraw.DrawLine(Points[i].GetPosition(), Points[i+step].GetPosition(), 0.01f, UltiDraw.Black);
			}
			
			//Directions
			for(int32 i=0; i<Points.Num(); i+=step) 
			{
				//UltiDraw.DrawLine(Points[i].GetPosition(), Points[i].GetPosition() + 0.25f*Points[i].GetDirection(), 0.025f, 0f, UltiDraw.Orange.Transparent(0.75f));
			}

			//Styles
			if (Styles.Num() > 0)
			{
				for(int32 i=0; i<Points.Num(); i+=step) 
				{
					float r = 0.0f;
					float g = 0.0f;
					float b = 0.0f;
					for (int32 j=0; j<Points[i].Styles.Num(); j++)
					{
						r += Points[i].Styles[j] * colors[j].R;
						g += Points[i].Styles[j] * colors[j].G;
						b += Points[i].Styles[j] * colors[j].B;
					}
					//FColor color(r,g,b,1.0f);
					//UltiDraw.DrawCube(Points[i].GetPosition(), Points[i].GetRotation(), 0.05f, color);
				}
			}

			//Positions
			for(int i=0; i<Points.Num(); i+=step) 
			{
				//UltiDraw.DrawCircle(Points[i].GetPosition(), 0.025f, UltiDraw.Black);
			}

			//Phase
			for(int i=0; i<Points.Num(); i+=step) 
			{
				//UltiDraw.DrawArrow(Points[i].GetPosition(), Points[i].GetPosition() + Points[i].Phase*FVector.up, 0.8f, 0.025f, 0.05f, UltiDraw.IndianRed.Transparent(0.5f));
			}
		}
	};
}
