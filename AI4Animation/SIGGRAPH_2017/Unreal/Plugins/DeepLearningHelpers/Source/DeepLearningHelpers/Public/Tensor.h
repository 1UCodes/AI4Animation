// juaxix - 2020 - xixgames

#pragma once

#include "CoreMinimal.h"
#include "EigenLibrary.h"

class Tensor
{
public:
	TSharedPtr<MatrixXf> Ptr;
	FString ID;

	Tensor() = default;

	Tensor(int rows, int cols, const FString& id)
	{
		Ptr = UEigenLibrary::CreateShared(rows, cols);
		ID = id;
		m_Deleted = false;
	}

	~Tensor()
	{
		Delete();
	}

	void Delete()
	{
		if ( !m_Deleted && Ptr)
		{
			//UEigenLibrary::Delete(Ptr);
			Ptr.Reset();
			m_Deleted = true;
		}
	}

	int GetRows()
	{
		return UEigenLibrary::GetRows(Ptr.Get());
	}

	int GetCols()
	{
		return UEigenLibrary::GetCols(Ptr.Get());
	}

	void SetZero()
	{
		UEigenLibrary::SetZero(Ptr.Get());
	}

	void SetSize(int rows, int cols)
	{
		UEigenLibrary::SetSize(Ptr.Get(), rows, cols);
	}

	void SetValue(int row, int col, float value)
	{
		if(row >= GetRows() || col >= GetCols())
		{
			UE_LOG(LogProcess, Display, TEXT("Setting out of bounds at [%d,%d]"), row, col);
			return;
		}

		UEigenLibrary::SetValue(Ptr.Get(), row, col, value);
	}

	float GetValue(int row, int col)
	{
		if(row >= GetRows() || col >= GetCols())
		{
			UE_LOG(LogProcess, Display, TEXT("Getting out of bounds at [%d,%d]"), row, col);
			return 0.0f;
		}

		return UEigenLibrary::GetValue(Ptr.Get(), row, col);
	}

	static Tensor* Add(Tensor* lhs, Tensor* rhs, Tensor* pOUT)
	{
		if(lhs->GetRows() != rhs->GetRows() || lhs->GetCols() != rhs->GetCols())
		{
			UE_LOG(LogProcess, Display, TEXT("Incompatible tensor dimensions."));
		}
		else
		{
			UEigenLibrary::Add(lhs->Ptr.Get(), rhs->Ptr.Get(), pOUT->Ptr.Get());
		}

		return pOUT;
	}

	static Tensor* Subtract(Tensor* lhs, Tensor* rhs, Tensor* pOUT)
	{
		if(lhs->GetRows() != rhs->GetRows() || lhs->GetCols() != rhs->GetCols())
		{
			UE_LOG(LogProcess, Display, TEXT("Incompatible tensor dimensions."));
		}
		else
		{
			UEigenLibrary::Subtract(lhs->Ptr.Get(), rhs->Ptr.Get(), pOUT->Ptr.Get());
		}

		return pOUT;
	}

	static Tensor* Product(Tensor* lhs, Tensor* rhs, Tensor* pOUT)
	{
		if(lhs->GetCols() != rhs->GetRows())
		{
			UE_LOG(LogProcess, Display, TEXT("Incompatible tensor dimensions."));
		}
		else
		{
			UEigenLibrary::Product(lhs->Ptr.Get(), rhs->Ptr.Get(), pOUT->Ptr.Get());
		}
		return pOUT;
	}

	static Tensor* Scale(Tensor* lhs, float value, Tensor* pOUT)
	{
		UEigenLibrary::Scale(lhs->Ptr.Get(), value, pOUT->Ptr.Get());
		return pOUT;
	}

	static Tensor* PointwiseProduct(Tensor* lhs, Tensor* rhs, Tensor* pOUT)
	{
		if(lhs->GetRows() != rhs->GetRows() || lhs->GetCols() != rhs->GetCols())
		{
			UE_LOG(LogProcess, Display, TEXT("Incompatible tensor dimensions."));
		}
		else
		{
			UEigenLibrary::PointwiseProduct(lhs->Ptr.Get(), rhs->Ptr.Get(), pOUT->Ptr.Get());
		}
		return pOUT;
	}

	static Tensor* PointwiseQuotient(Tensor* lhs, Tensor* rhs, Tensor* pOUT)
	{
		if(lhs->GetRows() != rhs->GetRows() || lhs->GetCols() != rhs->GetCols())
		{
			UE_LOG(LogProcess, Display, TEXT("Incompatible tensor dimensions."));
		}
		else
		{
			UEigenLibrary::PointwiseQuotient(lhs->Ptr.Get(), rhs->Ptr.Get(), pOUT->Ptr.Get());
		}
		return pOUT;
	}

	static Tensor* PointwiseAbsolute(Tensor* tensor_in, Tensor* tensor_out)
	{
		UEigenLibrary::PointwiseAbsolute(tensor_in->Ptr.Get(), tensor_out->Ptr.Get());
		return tensor_out;
	}

	float RowMean(int row)
	{
		if(row >= GetRows())
		{
			UE_LOG(LogProcess, Display, TEXT("Accessing out of bounds."));
			return 0.0f;
		}
		return UEigenLibrary::RowMean(Ptr.Get(), row);
	}

	float ColMean(int col)
	{
		if(col >= GetCols())
		{
			UE_LOG(LogProcess, Display, TEXT("Accessing out of bounds."));
			return 0.0f;
		}
		return UEigenLibrary::ColMean(Ptr.Get(), col);
	}

	float RowStd(int row)
	{
		if(row >= GetRows())
		{
			UE_LOG(LogProcess, Display, TEXT("Accessing out of bounds."));
			return 0.0f;
		}
		return UEigenLibrary::RowStd(Ptr.Get(), row);
	}

	float ColStd(int col)
	{
		if(col >= GetCols())
		{
			UE_LOG(LogProcess, Display, TEXT("Accessing out of bounds."));
			return 0.0f;
		}
		return UEigenLibrary::ColStd(Ptr.Get(), col);
	}

	float RowSum(int row)
	{
		if(row >= GetRows())
		{
			UE_LOG(LogProcess, Display, TEXT("Accessing out of bounds."));
			return 0.0f;
		}
		return UEigenLibrary::RowSum(Ptr.Get(), row);
	}

	float ColSum(int col)
	{
		if(col >= GetCols())
		{
			UE_LOG(LogProcess, Display, TEXT("Accessing out of bounds."));
			return 0.0f;
		}
		return UEigenLibrary::ColSum(Ptr.Get(), col);
	}

	void Print()
	{
		FString output = "";
		for(int i = 0; i < GetRows(); i++)
		{
			for(int j = 0; j < GetCols(); j++)
			{
				output += FString::SanitizeFloat(GetValue(i, j)) + " ";
			}
			output += "\n";
		}

		UE_LOG(LogProcess, Display, TEXT("%s"), *output);
	}

	bool operator==(const Tensor& rhs) const
	{
		return ID == rhs.ID && Ptr == rhs.Ptr &&
				 m_Deleted == rhs.m_Deleted;
	}

	bool operator==(Tensor* rhs) const
	{
		return rhs && ID == rhs->ID && Ptr == rhs->Ptr &&
				 m_Deleted == rhs->m_Deleted;
	}

private:
	bool m_Deleted;
};
