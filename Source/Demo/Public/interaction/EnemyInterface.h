// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// Reflection wrapper for enemy interactions.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

// Gameplay-facing enemy interaction interface.
class DEMO_API IEnemyInterface
{
	GENERATED_BODY()

	
public:

	/**
	 * Enables or disables the enemy highlight.
	 * @param bActive True to enable highlighting.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void ToggleHighlight(bool bActive);
};
