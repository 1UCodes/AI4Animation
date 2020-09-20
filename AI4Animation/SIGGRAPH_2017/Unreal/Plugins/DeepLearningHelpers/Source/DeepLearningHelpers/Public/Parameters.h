#pragma once

#include "CoreMinimal.h"
#include "Core/Public/Misc/CoreMisc.h"
#include "EigenLibrary.h"
#include "Engine/DataAsset.h"
#include "ArrayExtensions.h"

#include "Parameters.generated.h"

USTRUCT(BlueprintType)
struct FPVector
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Deep Learning|Parameters")
	TArray<float> Values;

	FPVector() = default;

	FPVector(int size)
	{
		Values.Reserve(size);
	}

	FPVector(FPVector* other)
	{
		Values = other->Values;
	}
};

USTRUCT(BlueprintType)
struct FPMatrix
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Deep Learning|Parameters")
	TArray<FPVector> Values;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Deep Learning|Parameters")
	int Rows;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Deep Learning|Parameters")
	int Cols;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Deep Learning|Parameters")
	FString ID;

	FPMatrix(): Values(), Rows(0), Cols(0), ID("")
	{
	}

	FPMatrix(int rows, int cols, const FString& id)
	{
		Rows = rows;
		Cols = cols;
		ID = id;
		Values.Reserve(rows);
		for(int i = 0; i < rows; i++)
		{
			Values.EmplaceAt(i, FPVector(cols));
		}
	}
};


UCLASS(BlueprintType, Blueprintable)
class UParameters : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Deep Learning|Parameters")
	TArray<FPMatrix> Matrices;

	UFUNCTION(BlueprintCallable, Category = "Deep Learning|Parameters")
	void Store(const FString& fn, int rows, int cols, const FString& id)
	{
		for(int i = 0; i < Matrices.Num(); i++)
		{
			if(Matrices[i].ID == id)
			{
				UE_LOG(LogProcess, Error, TEXT("FPMatrix with ID %s already contained."), *id);
				return;
			}
		}

		Matrices.Add(ReadBinary(fn, rows, cols, id));
	}

	UFUNCTION(BlueprintCallable, Category = "Deep Learning|Parameters")
	FPMatrix Load(const FString& id)
	{
		FPMatrix* matrix = Matrices.FindByPredicate([id](auto& x) { return x.ID == id; });
		if(matrix == nullptr)
		{
			UE_LOG(LogProcess, Error, TEXT("FPMatrix with ID %s not found."), *id);
			return FPMatrix(0, 0, TEXT(""));
		}
		return *matrix;
	}

	UFUNCTION(BlueprintCallable, Category = "Deep Learning|Parameters")
	void Clear()
	{
		Matrices.Empty();
	}

	UFUNCTION(BlueprintCallable, Category = "Deep Learning|Parameters")
	FPMatrix ReadBinary(const FString& fn, int rows, int cols, const FString& id)
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if(PlatformFile.FileExists(*fn))
		{
			FPMatrix matrix(rows, cols, id);
			IFileHandle* reader = PlatformFile.OpenRead(*fn, false);
			int errors = 0;
			constexpr int64 bytesToRead = sizeof(float);
			uint8* bytes = new uint8[bytesToRead];
			for(int x = 0; x < rows; x++)
			{
				for(int y = 0; y < cols; y++)
				{
					if (reader->Read(bytes, bytesToRead))
					{
						matrix.Values[x].Values[y] = *reinterpret_cast<float*>(bytes);
					}
					else
					{
						errors++;
					}
				}
			}
			delete[] bytes;

			if(errors > 0)
			{
				UE_LOG(LogProcess, Error, TEXT("There were %d errors reading file at path '%s'."), errors, *fn);
				return FPMatrix();
			}
			else
			{
				return matrix;
			}
		}

		UE_LOG(LogProcess, Error, TEXT("File at path '%s' does not exist."), *fn);
		return FPMatrix();
	}
};
