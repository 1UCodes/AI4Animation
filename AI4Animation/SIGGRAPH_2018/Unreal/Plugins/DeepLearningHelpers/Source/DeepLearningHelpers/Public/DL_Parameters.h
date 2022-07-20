#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
//#include "Utility/ArrayExtensions.h"

#include "DL_Parameters.generated.h"

USTRUCT(BlueprintType)
struct FPVector
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Parameters")
	TArray<float> Values;

	FPVector() = default;

	FPVector(int32 Size)
	{
		Values.SetNumZeroed(Size, true);
	}
};

USTRUCT(BlueprintType)
struct FPMatrix
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Parameters")
	TArray<FPVector> Values;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Parameters")
	int32 Rows = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Parameters")
	int32 Cols = 0;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Parameters")
	FString ID = TEXT("");

	FPMatrix(int32 InRows = 0, int32 InCols = 0, const FString& InID = TEXT(""))
	{
		Rows = InRows;
		Cols = InCols;
		ID = InID;
		Values.Reserve(InRows);
		for(int32 Index = 0; Index < InRows; Index++)
		{
			Values.EmplaceAt(Index, FPVector(InCols));
		}
	}
};

UCLASS(BlueprintType, Blueprintable)
class UDL_Parameters : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI|Deep Learning|Parameters")
	TArray<FPMatrix> Matrices;

	UFUNCTION(BlueprintCallable, Category = "AI|Deep Learning|Parameters")
	void Store(const FString& FunctionName, const int32 InRows, const int32 InCols, const FString& InID)
	{
		if (Matrices.FindByPredicate([&InID](const FPMatrix& M) -> bool {return InID == M.ID;}))
		{
			UE_LOG(LogProcess, Error, TEXT("FPMatrix with ID %s already contained."), *InID);
			return;
		}

		Matrices.Add(ReadBinary(FunctionName, InRows, InCols, InID));
	}

	UFUNCTION(BlueprintCallable, Category = "AI|Deep Learning|Parameters")
	void Save(){ SaveConfig(); }

	UFUNCTION(BlueprintCallable, Category = "AI|Deep Learning|Parameters")
	const FPMatrix& Load(const FString& ID)
	{
		const FPMatrix* Matrix = Matrices.FindByPredicate([&ID](auto& M) { return M.ID == ID; });
		if (Matrix == nullptr)
		{
			static const FPMatrix EmptyMatrix = FPMatrix();
			UE_LOG(LogProcess, Error, TEXT("FPMatrix with ID %s not found."), *ID);
			return EmptyMatrix;
		}

		return *Matrix;
	}

	UFUNCTION(BlueprintCallable, Category = "AI|Deep Learning|Parameters")
	void Clear()
	{
		Matrices.Empty(Matrices.Num());
	}

	UFUNCTION(BlueprintCallable, Category = "AI|Deep Learning|Parameters")
	FPMatrix ReadBinary(const FString& FunctionName, int32 Rows, int32 Cols, const FString& ID)
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (PlatformFile.FileExists(*FunctionName))
		{
			FPMatrix Matrix(Rows, Cols, ID);
			IFileHandle* Reader = PlatformFile.OpenRead(*FunctionName, false);
			int32 Errors = 0;
			constexpr size_t BytesToRead = sizeof(float);
			
			uint8* Bytes = new uint8[BytesToRead];
			for (int32 x = 0; x < Rows; x++)
			{
				for (int32 y = 0; y < Cols; y++)
				{
					if (Reader->Read(Bytes, BytesToRead))
					{
						Matrix.Values[x].Values[y] = *reinterpret_cast<float*>(Bytes);
					}
					else
					{
						Errors++;
					}
				}
			}

			delete[] Bytes;

			if (Errors > 0)
			{
				UE_LOG(LogProcess, Error, TEXT("There were %d errors reading file at path '%s'."), Errors, *FunctionName);
				return FPMatrix();
			}

			return Matrix;
		}
		
		UE_LOG(LogProcess, Error, TEXT("File at path '%s' does not exist."), *FunctionName);
		return FPMatrix();
	}

};
