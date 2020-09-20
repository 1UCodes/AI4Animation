#pragma once

#include "CoreMinimal.h"
#include "EigenLibrary.h"
#include "Engine/DataAsset.h"
#include "ArrayExtensions.h"

#include "Parameters.generated.h"

USTRUCT(BlueprintType)
struct FPVector
{
	GENERATED_BODY()
public:
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parameters")
	TArray<FPVector> Values;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parameters")
	int Rows;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parameters")
	int Cols;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parameters")
	FString ID;

	FPMatrix(): Values(), Rows(0), Cols(0), ID("")
	{
	}

	FPMatrix(int rows, int cols, FString id)
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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Parameters")
	TArray<FPMatrix> Matrices;

	UFUNCTION(BlueprintCallable, Category = "Parameters")
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

	UFUNCTION(BlueprintCallable, Category = "Parameters")
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

	UFUNCTION(BlueprintCallable, Category = "Parameters")
	void Clear()
	{
		Matrices.Empty();
	}

	UFUNCTION(BlueprintCallable, Category = "Parameters")
	FPMatrix ReadBinary(const FString& fn, int rows, int cols, const FString& id)
	{
		UE_LOG(LogProcess, Warning, TEXT("Not implemented yet"));
		/** @TODO */
		/*if(File.Exists(fn))
		{
			 FPMatrix matrix = new FPMatrix(rows, cols, id);
			 BinaryReader reader = new BinaryReader(File.Open(fn, FileMode.Open));
			 int errors = 0;
			 for(int x=0; x<rows; x++) {
				  for(int y=0; y<cols; y++) {
				 try {
					  matrix.Values[x].Values[y] = reader.ReadSingle();
				 } catch {
					  errors += 1;
				 }
				  }
			 }
			 reader.Close();
			if(errors > 0) {
			 Debug.Log("There were " + errors + " errors reading file at path " + fn + ".");
			 return null;
			}
			else
			{
				return matrix;
			}
		}
		else
		{
				Debug.Log("File at path " + fn + " does not exist.");
				return null;
		}*/
		return FPMatrix(0, 0, TEXT(""));
	}

};
