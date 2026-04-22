// Rylan

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PlayerGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UPlayerGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
		UPROPERTY(EditDefaultsOnly, Category ="Input")
	FGameplayTag StartupInputTag;
	
	
};
