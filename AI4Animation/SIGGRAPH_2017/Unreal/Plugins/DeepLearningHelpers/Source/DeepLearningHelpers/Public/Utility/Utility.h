// juaxix - 2020-2022 - xixgames

#pragma once

#include "CoreMinimal.h"

namespace DeepLearningHelpers
{
	namespace Utility
	{
		inline float Interpolate(float From, float To, float Amount)
		{
			Amount = FMath::Clamp(Amount, 0.0f, 1.0f);
			return (1.0f - Amount) * From + Amount * To;
		}

		inline FVector Interpolate(const FVector& From, const FVector& To, float Amount)
		{
			Amount = FMath::Clamp(Amount,0.0f,1.0f);
			return (1.0f - Amount) * From + Amount * To;
		}

		inline FQuat AngleAxis(float Degrees, FVector Axis)
		{
			if (Axis.SizeSquared() == 0.0f)
			{
				return FQuat::Identity;
			}

			FQuat Result;
			static constexpr float Deg2Rad = PI / 180.0f;
			float Radians = Degrees * Deg2Rad;
			Radians *= 0.5f;
			Axis.Normalize();
			Axis = Axis * FMath::Sin(Radians);
			Result.X = Axis.X;
			Result.Y = Axis.Y;
			Result.Z = Axis.Z;
			Result.W = FMath::Cos(Radians);
			Result.Normalize();

			return Result;
		}

		inline FQuat QuaternionAverage(const TArray<FQuat>& Quaternions)
		{
			FVector Forward = FVector::ZeroVector;
			FVector Upwards = FVector::ZeroVector;

			for (int32 i = 0; i < Quaternions.Num(); i++) 
			{
				Forward += Quaternions[i] * FVector::ForwardVector;
				Upwards += Quaternions[i] * FVector::UpVector;
			}

			Forward /= Quaternions.Num();
			Upwards /= Quaternions.Num();
			
			return FRotationMatrix::MakeFromX(Upwards - Forward).ToQuat();
		}

		inline void Normalise(TArray<float>& Values)
		{
			float Sum = 0.0f;
			for (int32 i = 0; i < Values.Num(); i++) 
			{
				Sum += FMath::Abs(Values[i]);
			}

			if (Sum != 0.0f) 
			{
				for (int32 i = 0; i < Values.Num(); i++) 
				{
					Values[i] = FMath::Abs(Values[i]) / Sum;
				}
			}
		}

		inline float Normalise(float value, float valueMin, float valueMax, float resultMin, float resultMax)
		{
			if (valueMax - valueMin != 0.0f) 
			{
				return (value - valueMin) / (valueMax - valueMin) * (resultMax - resultMin) + resultMin;
			}

			//Not possible to normalise input value.
			return value;
		}

		inline FVector GetPositionFromMatrix(const FMatrix& Matrix)
		{
			return FVector(Matrix.M[0][3], Matrix.M[1][3], Matrix.M[2][3]);
		}

		inline FRotator GetRotationFromMatrix(const FMatrix& Matrix)
		{
			return FRotationMatrix::MakeFromX(
					Matrix.GetColumn(1).GetSafeNormal() - Matrix.GetColumn(2).GetSafeNormal()
				   ).Rotator();
			// original from unity:
			// return Quaternion.LookRotation(matrix.GetColumn(2).normalized, matrix.GetColumn(1).normalized);
		}

	}
}
