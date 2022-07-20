// juaxix - 2020-2022 - xixgames
#pragma once
#include "CoreMinimal.h"

namespace DeepLearningHelpers
{
	class DEEPLEARNINGHELPERS_API ArrayExtensions
	{
	public:
		template <typename T>
		static void Add(TArray<T>& Array, T Element)
		{
			Expand(Array);
			Array[Array.Num() - 1] = Element;
		}

		template <typename T>
		static void Insert(TArray<T>& Array, T Element, int32 Index)
		{
			if (Index >= 0 && Index < Array.Num())
			{
				Expand(Array);
				for(int32 i = Array.Num() - 1; i > Index; --i)
				{
					Array[i] = Array[i - 1];
				}
				Array[Index] = Element;
			}
		}

		template <typename T>
		static void RemoveAt(TArray<T>& Array, int32 Index)
		{
			if (Index >= 0 && Index < Array.Num())
			{
				for(int32 i = Index; i < Array.Num() - 1; i++)
				{
					Array[i] = Array[i + 1];
				}
				Shrink(Array);
			}
		}

		template <typename T>
		static void Remove(TArray<T>& Array, T Element)
		{
			RemoveAt(Array, FindIndex(Array, Element));
		}

		template <typename T>
		static void Expand(TArray<T>& Array)
		{
			Array.Reserve(Array.Num() + 1);
		}

		template <typename T>
		static void Shrink(TArray<T>& Array)
		{
			if (Array.Num() > 0)
			{
				Resize(Array.Num - 1);
			}
		}

		template <typename T>
		static void Resize(TArray<T>& Array, int32 Size)
		{
			const int32 Num = Array.Num();
			if (Num > Size)
			{
				Array.RemoveAt(FMath::Abs(Size - 1), Num - Size, true);
			}
			else if (Num < Size)
			{
				 Array.Reserve(Size);
			}
		}

		template <typename T>
		static void Clear(TArray<T>& Array)
		{
			Array.Empty(0);
		}

		template <typename T>
		static int32 FindIndex(const TArray<T>& Array, T Element)
		{
			return Array.IndexOfByPredicate([Element](const T& x) { return x.Equals(Element); });
		}

		template <typename T>
		static T Find(TArray<T>& Array, T Element)
		{
			return Array.Find(Element);
		}

		template <typename T>
		static bool Contains(TArray<T>& Array, T& Element)
		{
			return Array.Contains(Element);
		}

		template <typename T>
		static TArray<T> Concat(const TArray<T>& lhs, const TArray<T>& rhs)
		{
			TArray<T> result;
			result.Reserve(lhs.Num() + rhs.Num());
			result.Append(lhs.GetData(), lhs.Num());
			result.Append(rhs.GetData(), rhs.Num());
			return result;
		}

		template <typename T>
		static TArray<T> Concat(T& lhs, const TArray<T>& rhs)
		{
			TArray<T> clone = rhs;
			clone.Insert(lhs, 0);
			return clone;
		}

		template <typename T>
		static TArray<T> Concat(const TArray<T>& lhs, T& rhs)
		{
			TArray<T> clone = lhs;
			clone.Append(rhs);
			return clone;
		}

		static TArray<float> Add(const TArray<float>& lhs, const TArray<float>& rhs)
		{
			if (lhs.Num() != rhs.Num())
			{
				UE_LOG(LogProcess, Display, TEXT("Incompatible array dimensions."));
				return TArray<float>();
			}
			TArray<float> result;
			result.Reserve(lhs.Num());
			for(int32 i = 0; i < result.Num(); i++)
			{
				result[i] = lhs[i] + rhs[i];
			}
			return result;
		}

		static TArray<float> Add(const TArray<float>& lhs, float value)
		{
			TArray<float> result;
			result.Reserve(lhs.Num());
			for(int32 i = 0; i < result.Num(); i++)
			{
				result[i] = lhs[i] + value;
			}
			return result;
		}

		static TArray<float> Sub(const TArray<float>& lhs, const TArray<float>& rhs)
		{
			if (lhs.Num() != rhs.Num())
			{
				UE_LOG(LogProcess, Warning, TEXT("Incompatible array dimensions."));
				return TArray<float>();
			}
			TArray<float> result;
			result.Reserve(lhs.Num());
			for(int32 i = 0; i < result.Num(); i++)
			{
				result[i] = lhs[i] - rhs[i];
			}
			return result;
		}

		static TArray<float> Sub(const TArray<float>& lhs, float value)
		{
			TArray<float> result;
			result.Reserve(lhs.Num());
			for(int32 i = 0; i < result.Num(); i++)
			{
				result[i] = lhs[i] - value;
			}
			return result;
		}

		static int32 Sum(const TArray<int32>& values)
		{
			int32 sum = 0;
			for(int32 i = 0; i < values.Num(); i++)
			{
				sum += values[i];
			}

			return sum;
		}

		static float Sum(const TArray<float>& values)
		{
			float sum = 0.0f;
			for(int32 i = 0; i < values.Num(); i++)
			{
				sum += values[i];
			}
			return sum;
		}

		static double Sum(const TArray<double>& values)
		{
			double sum = 0.0;
			for(int32 i = 0; i < values.Num(); i++)
			{
				sum += values[i];
			}
			return sum;
		}

		static int32 AbsSum(const TArray<int32>& values)
		{
			int32 sum = 0;
			for(int32 i = 0; i < values.Num(); i++)
			{
				sum += FMath::Abs(values[i]);
			}
			return sum;
		}

		static float AbsSum(const TArray<float>& values)
		{
			float sum = 0.0f;
			for(int32 i = 0; i < values.Num(); i++)
			{
				sum += FMath::Abs(values[i]);
			}
			return sum;
		}

		static double AbsSum(const TArray<double>& values)
		{
			double sum = 0.0;
			for(int32 i = 0; i < values.Num(); i++)
			{
				sum += FMath::Abs(values[i]);
			}
			return sum;
		}

		static int32 Min(const TArray<int32>& values)
		{
			if (values.Num() == 0)
			{
				return 0;
			}
			int32 min = INT_MAX;
			for(int32 i = 0; i < values.Num(); i++)
			{
				min = FMath::Min(min, values[i]);
			}
			return min;
		}

		static float Min(const TArray<float>& values)
		{
			if (values.Num() == 0)
			{
				return 0.0f;
			}
			float min = TNumericLimits<float>::Max();
			for(int32 i = 0; i < values.Num(); i++)
			{
				min = FMath::Min(min, values[i]);
			}
			return min;
		}

		static double Min(const TArray<double>& values)
		{
			if (values.Num() == 0)
			{
				return 0.0;
			}
			double min = TNumericLimits<double>::Max();
			for(int32 i = 0; i < values.Num(); i++)
			{
				min = FMath::Min(min, values[i]);
			}
			return min;
		}

		static int32 Max(const TArray<int32>& values)
		{
			if (values.Num() == 0)
			{
				return 0;
			}
			int32 max = INT_MAX;
			for(int32 i = 0; i < values.Num(); i++)
			{
				max = FMath::Max(max, values[i]);
			}
			return max;
		}

		static float Max(const TArray<float>& values)
		{
			if (values.Num() == 0)
			{
				return 0.0f;
			}
			float max = TNumericLimits<float>::Max();
			for(int32 i = 0; i < values.Num(); i++)
			{
				max = FMath::Max(max, values[i]);
			}
			return max;
		}

		static double Max(const TArray<double>& values)
		{
			if (values.Num() == 0)
			{
				return 0.0;
			}
			double max = TNumericLimits<double>::Max();
			for(int32 i = 0; i < values.Num(); i++)
			{
				max = FMath::Max(max, values[i]);
			}
			return max;
		}

		static float Mean(const TArray<int32>& values)
		{
			if (values.Num() == 0)
			{
				return 0;
			}
			float mean = 0.f;
			float args = 0.f;
			for(int32 i = 0; i < values.Num(); i++)
			{
				mean += values[i];
				args += 1.f;
			}
			mean /= args;
			return mean;
		}

		static float Mean(const TArray<float>& values)
		{
			if (values.Num() == 0)
			{
				return 0.f;
			}
			float mean = 0.f;
			float args = 0.f;
			for(int32 i = 0; i < values.Num(); i++)
			{
				mean += values[i];
				args += 1.f;
			}
			mean /= args;
			return mean;
		}

		static double Mean(const TArray<double>& values)
		{
			if (values.Num() == 0)
			{
				return 0.0;
			}
			double mean = 0.0;
			double args = 0.0;
			for(int32 i = 0; i < values.Num(); i++)
			{
				mean += values[i];
				args += 1.0;
			}
			mean /= args;
			return mean;
		}

		static float Sigma(const TArray<int32>& values)
		{
			if (values.Num() == 0)
			{
				return 0.f;
			}
			float variance = 0.f;
			float mean = Mean(values);
			float args = 0.f;
			for(int32 i = 0; i < values.Num(); i++)
			{
				variance += FMath::Pow(values[i] - mean, 2.f);
				args += 1.f;
			}
			variance /= args;
			return FMath::Sqrt(variance);
		}

		static float Sigma(const TArray<float>& values)
		{
			if (values.Num() == 0)
			{
				return 0.f;
			}
			float variance = 0.f;
			float mean = Mean(values);
			float args = 0.0f;
			for(int32 i = 0; i < values.Num(); i++)
			{
				variance += FMath::Pow(values[i] - mean, 2.0f);
				args += 1.f;
			}
			variance /= args;
			return FMath::Sqrt(variance);
		}

		static double Sigma(const TArray<double>& values)
		{
			if (values.Num() == 0)
			{
				return 0.0;
			}
			double variance = 0.0;
			double mean = Mean(values);
			double args = 1.0;
			for(int32 i = 0; i < values.Num(); i++)
			{
				variance += FMath::Pow(values[i] - mean, 2.0);
				args += 1.0;
			}
			variance /= args;
			return FMath::Sqrt(variance);
		}

		static void Zero(TArray<float>& values)
		{
			values.SetNumZeroed(values.Num(), false);
		}

		static void Print(const TArray<double>& values)
		{
			FString Output = TEXT("[");
			for(int32 i = 0; i < values.Num(); i++)
			{
				Output += FString::SanitizeFloat(values[i]) + (i == values.Num() - 1 ? "]" : ", ");
			}

			UE_LOG(LogProcess, Display, TEXT("%s"), *Output);
		}

		static void Print(const TArray<float>& values)
		{
			FString output = TEXT("[");
			for(int32 i = 0; i < values.Num(); i++)
			{
				output += FString::SanitizeFloat(values[i]) + (i == values.Num() - 1 ? "]" : ", ");
			}
			UE_LOG(LogProcess, Display, TEXT("%s"), *output);
		}

		static void Print(const TArray<int32>& values)
		{
			FString output = TEXT("[");
			for(int32 i = 0; i < values.Num(); i++)
			{
				output += FString::FromInt(values[i]) + (i == values.Num() - 1 ? "]" : ", ");
			}
			UE_LOG(LogProcess, Display, TEXT("%s"), *output);
		}
	};
}