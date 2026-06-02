// Rylan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "PlayerAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UPlayerAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
