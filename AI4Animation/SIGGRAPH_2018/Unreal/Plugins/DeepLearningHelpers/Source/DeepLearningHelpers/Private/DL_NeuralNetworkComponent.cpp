// juaxix - 2020-2022 - xixgames

#include "DL_NeuralNetworkComponent.h"

#include "Kismet/KismetSystemLibrary.h"

UDL_NeuralNetworkComponent::UDL_NeuralNetworkComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Folder = UKismetSystemLibrary::GetProjectContentDirectory() / TEXT("DeepLearning") / TEXT("trained");
	if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*Folder))
	{
		UE_LOG(LogTemp, Warning, TEXT("Folder %s does not exist, please make the appropiate link"), *Folder);
	}
}
