// Rylan


#include "PlayerAssetManager.h"

#include "PlayerGameplayTags.h"


UPlayerAssetManager& UPlayerAssetManager::Get()
{
	check(GEngine);
	UPlayerAssetManager* PlayerAssetManager = Cast<UPlayerAssetManager>(GEngine->AssetManager);
	return *PlayerAssetManager;
}

void UPlayerAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FPlayerGameplayTags::InitializeNativeGameplayTags();
}
