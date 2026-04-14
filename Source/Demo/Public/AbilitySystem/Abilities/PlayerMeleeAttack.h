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

	// 🔴 之前写的射线检测与伤害应用函数
	void PerformMeleeTraceAndApplyDamage();
    
	// 伤害的 GE 和 射线参数...
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TSubclassOf<class UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
	float TraceDistance = 150.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
	float TraceRadius = 45.f;
};
