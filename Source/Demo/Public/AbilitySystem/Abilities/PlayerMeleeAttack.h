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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	FGameplayTag HitEventTag;
	
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnHitEventReceived(FGameplayEventData Payload);
	
	UPROPERTY(EditDefaultsOnly, Category ="Damage")
	FScalableFloat Damage;
	void PerformMeleeTraceAndApplyDamage();
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
	float TraceDistance = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
	float TraceRadius = 45.f;

	// Timer-based fallback: fires hit trace even without matching anim notify event
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Timing")
	float HitTraceDelay = 0.5f;

	FTimerHandle HitTraceTimerHandle;
	bool bTraceAlreadyFired = false;

	UFUNCTION()
	void OnHitTraceTimerFired();
};