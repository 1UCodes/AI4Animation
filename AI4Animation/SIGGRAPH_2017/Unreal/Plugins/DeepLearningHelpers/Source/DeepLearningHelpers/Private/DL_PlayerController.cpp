// juaxix - 2020-2022 - xixgames


#include "DL_PlayerController.h"

bool FDLPlayerInputStyle::Query() const
{
	if (Keys.Num() == 0) 
	{
		return false;
	}
		
	bool bActive = false;
	// TODO Cache the input that should be bind in the input component
	for (int32 i=0; i < Keys.Num(); i++)
	{
		if (!Keys[i].bNegate) 
		{
			if (Keys[i].Key.GetFName() == NAME_None) 
			{
				//TODO
				//if(!InputHandler.anyKey) 
				{
					bActive = true;
				}
			} else {
				// TODO
				// if(InputHandler.GetKey(Keys[i])) 
				{
					//active = true;
				}
			}
		}
	}
	// TODO Cache the input that should be bind in the input component
	for (int32 i=0; i<Keys.Num(); i++)
	{
		if (Keys[i].bNegate) 
		{
			if (Keys[i].Key.GetFName() == NAME_None) 
			{
				// TODO
				//if(!InputHandler.anyKey)
				{
					bActive = false;
				}
			} else {
				//if(InputHandler.GetKey(Keys[i]))
				{
					bActive = false;
				}
			}
		}
	}

	return bActive;
}

void FDLPlayerInputStyle::SetKeyCount(int32 InCount)
{
	InCount = FMath::Max(InCount, 0);
	if(Keys.Num() != InCount) 
	{
		Keys.SetNum(InCount);
	}
}

ADL_PlayerController::ADL_PlayerController()
	: Styles(TArray<FDLPlayerInputStyle>())
{
}

void ADL_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("InGameMenu", IE_Pressed, this, &ADL_PlayerController::ToggleInGameMenu);

}

TArray<float> ADL_PlayerController::GetStyle() const
{
	TArray<float> Style;
	Style.Reserve(Styles.Num());

	for (int32 i = 0; i < Styles.Num(); i++) 
	{
		Style.EmplaceAt(i, Styles[i].Query() ? 1.0f : 0.0f);
	}

	return Style;
}

TArray<FString> ADL_PlayerController::GetNames() const
{
	TArray<FString> Names;
	Names.SetNum(Styles.Num());
	for (int32 i = 0; i < Names.Num(); i++) 
	{
		Names[i] = Styles[i].Name;
	}

	return Names;
}

FVector ADL_PlayerController::QueryMove() const
{
	FVector Move = FVector::ZeroVector;
	// TODO Cache the input that should be bind in the input component
	/*if(InputHandler.GetKey(Forward)) 
		{
			Move.Y += 1f;
		}
		if(InputHandler.GetKey(Back))
		{
			Move.Y -= 1f;
		}
		if(InputHandler.GetKey(Left))
		{
			Move.X -= 1f;
		}
		if(InputHandler.GetKey(Right))
		{
			Move.X += 1f;
		}
		*/

	return Move;
}

float ADL_PlayerController::QueryTurn() const
{
	float Turn = 0.0f;
	// TODO Cache the input that should be bind in the input component
	/*if(InputHandler.GetKey(TurnLeft)) 
		{
			Turn -= 1f;
		}

		if(InputHandler.GetKey(TurnRight)) 
		{
			Turn += 1f;
		}*/

	return Turn;
}

bool ADL_PlayerController::QueryAny() const
{
	for (int32 i=0; i < Styles.Num(); i++)
	{
		if (Styles[i].Query()) 
		{
			return true;
		}
	}

	return false;
}

float ADL_PlayerController::PoolBias(const TArray<float>& Weights) const
{
	float Bias = 0.0f;
	for(int32 i = 0; i < Weights.Num(); i++) 
	{
		float _bias = Styles[i].Bias;
		float max = 0.0f;

		// TODO Cache the input that should be bind in the input component
		for(int32 j = 0; j < Styles[i].Multipliers.Num(); j++) 
		{
			// TODO
			//if(InputHandler.GetKey(Styles[i].Multipliers[j].Key)) 
			{
				max = FMath::Max(max, Styles[i].Bias * Styles[i].Multipliers[j].Value);
			}
		}

		for(int32 j=0;  j < Styles[i].Multipliers.Num(); j++) 
		{
			// TODO
			//if(InputHandler.GetKey(Styles[i].Keys[k].Multipliers[j].Key)) 
			{
				_bias = FMath::Min(max, _bias * Styles[i].Multipliers[j].Value);
			}
		}

		Bias += Weights[i] * _bias;
		
	}

	return Bias;
}

void ADL_PlayerController::ToggleInGameMenu()
{
	// TODO
}