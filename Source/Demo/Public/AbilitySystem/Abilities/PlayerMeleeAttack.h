// Rylan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PlayerGameplayAbility.h"
#include "PlayerMeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UPlayerMeleeAttack : public UPlayerGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPlayerMeleeAttack();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* AttackMontage;

	
	UFUNCTION()
	void OnMontageCompleted();
};
