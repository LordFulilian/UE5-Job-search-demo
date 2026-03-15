// Rylan

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "PlayerAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UPlayerAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	static UPlayerAssetManager& Get();	
	
protected:
	virtual  void StartInitialLoading() override;
};
