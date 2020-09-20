// juaxix - 2020 - xixgames

#pragma once
#include "Core.h"
#include "Runtime/Engine/Classes/Kismet/BlueprintFunctionLibrary.h"
#ifdef _EG_WINDOWS_PLATFORM
#include "Runtime/Core/Public/Windows/AllowWindowsPlatformTypes.h"
#include "Runtime/Core/Public/Windows/PreWindowsApi.h"
#endif
#ifdef __clang__
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif
#define _CRT_SECURE_NO_WARNINGS
#if !defined(_EG_IPHONE_PLATFORM) && ! defined (_EG_IMAC_PLATFORM)
#pragma warning (disable: 4263)
#pragma warning (disable: 4264)
#endif
#include "ThirdParty/Eigen/Dense"

#if !defined(_EG_IPHONE_PLATFORM) && ! defined (_EG_IMAC_PLATFORM)
#pragma warning (default: 4263)
#pragma warning (default: 4264)
#endif
#ifdef _EG_WINDOWS_PLATFORM
#include "Runtime/Core/Public/Windows/PostWindowsApi.h"
#include "Runtime/Core/Public/Windows/HideWindowsPlatformTypes.h"
#endif

#include "EigenLibrary.generated.h"

using namespace Eigen;

UCLASS()
class EIGEN_API UEigenLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	static MatrixXf* Create(int rows, int cols)
	{
		return new MatrixXf(Eigen::MatrixXf::Zero(rows, cols));
	}

	static void Delete(MatrixXf* ptr)
	{
		delete(ptr);
	}

	static int GetRows(MatrixXf* ptr)
	{
		return (*ptr).rows();
	}

	static int GetCols(MatrixXf* ptr)
	{
		return (*ptr).cols();
	}

	static void SetZero(MatrixXf* ptr)
	{
		(*ptr).noalias() = (*ptr).Zero((*ptr).rows(), (*ptr).cols());
	}

	static void SetSize(MatrixXf* ptr, int rows, int cols)
	{
		(*ptr).conservativeResize(rows, cols);
	}

	static void Add(MatrixXf* lhs, MatrixXf* rhs, MatrixXf* out)
	{
		(*out).noalias() = *lhs + *rhs;
	}

	static void Subtract(MatrixXf* lhs, MatrixXf* rhs, MatrixXf* out)
	{
		(*out).noalias() = *lhs - *rhs;
	}

	static void Product(MatrixXf* lhs, MatrixXf* rhs, MatrixXf* out)
	{
		(*out).noalias() = *lhs * *rhs;
	}

	static void Scale(MatrixXf* lhs, float value, MatrixXf* out)
	{
		(*out).noalias() = *lhs * value;
	}

	static void SetValue(MatrixXf* ptr, int row, int col, float value)
	{
		(*ptr)(row, col) = value;
	}

	static float GetValue(MatrixXf* ptr, int row, int col)
	{
		return (*ptr)(row, col);
	}

	static void PointwiseProduct(MatrixXf* lhs, MatrixXf* rhs, MatrixXf* out)
	{
		(*out).noalias() = (*lhs).cwiseProduct(*rhs);
	}

	static void PointwiseQuotient(MatrixXf* lhs, MatrixXf* rhs, MatrixXf* out)
	{
		(*out).noalias() = (*lhs).cwiseQuotient(*rhs);
	}

	static void PointwiseAbsolute(MatrixXf* in, MatrixXf* out)
	{
		(*out).noalias() = (*in).cwiseAbs();
	}

	static float RowSum(MatrixXf* ptr, int row)
	{
		return (*ptr).row(row).sum();
	}

	static float ColSum(MatrixXf* ptr, int col)
	{
		return (*ptr).col(col).sum();
	}

	static float RowMean(MatrixXf* ptr, int row)
	{
		return (*ptr).row(row).mean();
	}

	static float ColMean(MatrixXf* ptr, int col)
	{
		return (*ptr).col(col).mean();
	}

	static float RowStd(MatrixXf* ptr, int row)
	{
		MatrixXf diff = (*ptr).row(row) -
				(*ptr).row(row).mean() * MatrixXf::Ones(1, (*ptr).rows());
		diff = diff.cwiseProduct(diff);
		return std::sqrt(diff.sum() / (*ptr).cols());
	}

	static float ColStd(MatrixXf* ptr, int col)
	{
		MatrixXf diff = (*ptr).col(col) -
				(*ptr).col(col).mean() * MatrixXf::Ones((*ptr).rows(), 1);
		diff = diff.cwiseProduct(diff);
		return std::sqrt(diff.sum() / (*ptr).rows());
	}

	static void Normalise(MatrixXf* ptr, MatrixXf* mean, MatrixXf* std, MatrixXf* out)
	{
		(*out).noalias() = (*ptr - *mean).cwiseQuotient(*std);
	}

	static void Renormalise(MatrixXf* ptr, MatrixXf* mean, MatrixXf* std, MatrixXf* out)
	{
		(*out).noalias() = (*ptr).cwiseProduct(*std) + *mean;
	}

	static void ILayer(MatrixXf* X, MatrixXf* W, MatrixXf* b)
	{
		(*X).noalias() = *W * *X + *b;
	}

	static void Layer(MatrixXf* in, MatrixXf* W, MatrixXf* b, MatrixXf* out)
	{
		(*out).noalias() = *W * *in + *b;
	}

	static void Blend(MatrixXf* in, MatrixXf* W, float w)
	{
		(*in).noalias() += w * *W;
	}

	static void BlendAll(MatrixXf* in, MatrixXf** W, float* w, int length)
	{
		if(length == 0)
		{
			SetZero(in);
		}
		else
		{
			switch(length)
			{
			case 1:
				(*in).noalias() = w[0] * *W[0];
				break;
			case 2:
				(*in).noalias() = w[0] * *W[0] + w[1] * *W[1];
				break;
			case 3:
				(*in).noalias() = w[0] * *W[0] + w[1] * *W[1] + w[2] * *W[2];
				break;
			case 4:
				(*in).noalias() = w[0] * *W[0] + w[1] * *W[1] + w[2] * *W[2] + w[3] * *W[3];
				break;
			case 5:
				(*in).noalias() = w[0] * *W[0] + w[1] * *W[1] + w[2] * *W[2] + w[3] * *W[3] + w[4] * *W[4];
				break;
			case 6:
				(*in).noalias() = w[0] * *W[0] + w[1] * *W[1] + w[2] * *W[2] + w[3] * *W[3] + w[4] * *W[4] + w[5] * *W[5];
				break;
			case 7:
				(*in).noalias() = w[0] * *W[0] + w[1] * *W[1] + w[2] * *W[2] + w[3] * *W[3] + w[4] * *W[4] + w[5] * *W[5] +
						w[6] * *W[6];
				break;
			case 8:
				(*in).noalias() = w[0] * *W[0] + w[1] * *W[1] + w[2] * *W[2] + w[3] * *W[3] + w[4] * *W[4] + w[5] * *W[5] +
						w[6] * *W[6] + w[7] * *W[7];
				break;
			case 9:
				(*in).noalias() = w[0] * *W[0] + w[1] * *W[1] + w[2] * *W[2] + w[3] * *W[3] + w[4] * *W[4] + w[5] * *W[5] +
						w[6] * *W[6] + w[7] * *W[7] + w[8] * *W[8];
				break;
			case 10:
				(*in).noalias() = w[0] * *W[0] + w[1] * *W[1] + w[2] * *W[2] + w[3] * *W[3] + w[4] * *W[4] + w[5] * *W[5] +
						w[6] * *W[6] + w[7] * *W[7] + w[8] * *W[8] + w[9] * *W[9];
				break;
			case 11:
				(*in).noalias() = w[0] * *W[0] + w[1] * *W[1] + w[2] * *W[2] + w[3] * *W[3] + w[4] * *W[4] + w[5] * *W[5] +
						w[6] * *W[6] + w[7] * *W[7] + w[8] * *W[8] + w[9] * *W[9] + w[10] * *W[10];
				break;
			case 12:
				(*in).noalias() = w[0] * *W[0] + w[1] * *W[1] + w[2] * *W[2] + w[3] * *W[3] + w[4] * *W[4] + w[5] * *W[5] +
						w[6] * *W[6] + w[7] * *W[7] + w[8] * *W[8] + w[9] * *W[9] + w[10] * *W[10] + w[11] * *W[11];
				break;
			default:
				{
					(*in).noalias() = w[0] * *W[0];
					for(int i = 1; i < length; i++)
					{
						(*in).noalias() += w[i] * *W[i];
					}
				}
				break;
			}
		}
	}

	static void RELU(MatrixXf* ptr)
	{
		(*ptr).noalias() = (*ptr).cwiseMax(0.0f);
	}

	static void ELU(MatrixXf* ptr)
	{
		(*ptr).noalias() = ((*ptr).array().cwiseMax(0.0f) + (*ptr).array().cwiseMin(0.0f).exp() - 1.0f).matrix();
		//int rows = (*ptr).rows();
		//for (int i = 0; i<rows; i++) {
		//	(*ptr)(i, 0) = (std::max)((*ptr)(i, 0), 0.0f) + std::exp((std::min)((*ptr)(i, 0), 0.0f)) - 1.0f;
		//}
	}

	static void Sigmoid(MatrixXf* ptr)
	{
		int rows = (*ptr).rows();
		for(int i = 0; i < rows; i++)
		{
			(*ptr)(i, 0) = 1.0f / (1.0f + std::exp(-(*ptr)(i, 0)));
		}
	}

	static void TanH(MatrixXf* ptr)
	{
		int rows = (*ptr).rows();
		for(int i = 0; i < rows; i++)
		{
			(*ptr)(i, 0) = std::tanh((*ptr)(i, 0));
		}
	}

	static void SoftMax(MatrixXf* ptr)
	{
		float frac = 0.0f;
		int rows = (*ptr).rows();
		for(int i = 0; i < rows; i++)
		{
			(*ptr)(i, 0) = std::exp((*ptr)(i, 0));
			frac += (*ptr)(i, 0);
		}
		for(int i = 0; i < rows; i++)
		{
			(*ptr)(i, 0) /= frac;
		}
	}

	static void LogSoftMax(MatrixXf* ptr)
	{
		float frac = 0.0f;
		int rows = (*ptr).rows();
		for(int i = 0; i < rows; i++)
		{
			(*ptr)(i, 0) = std::exp((*ptr)(i, 0));
			frac += (*ptr)(i, 0);
		}
		for(int i = 0; i < rows; i++)
		{
			(*ptr)(i, 0) = std::log((*ptr)(i, 0) / frac);
		}
	}

	static void SoftSign(MatrixXf* ptr)
	{
		int rows = (*ptr).rows();
		for(int i = 0; i < rows; i++)
		{
			(*ptr)(i, 0) /= 1 + std::abs((*ptr)(i, 0));
		}
	}

	static void Exp(MatrixXf* ptr)
	{
		int rows = (*ptr).rows();
		for(int i = 0; i < rows; i++)
		{
			(*ptr)(i, 0) = std::exp((*ptr)(i, 0));
		}
	}
};
