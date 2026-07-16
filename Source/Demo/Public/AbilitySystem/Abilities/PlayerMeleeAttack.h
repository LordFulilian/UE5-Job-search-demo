// Rylan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PlayerGameplayAbility.h"
#include "PlayerMeleeAttack.generated.h"

/**
 * */
UCLASS()
class DEMO_API UPlayerMeleeAttack : public UPlayerGameplayAbility
{
    GENERATED_BODY()
    
public:
    UPlayerMeleeAttack();
    
protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    
    // Animation and event tags.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    UAnimMontage* AttackMontage;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Tags")
    FGameplayTag HitEventTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Tags")
    FGameplayTag ComboEventTag; 

    // Optional gameplay event tag associated with buffered input.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Tags")
    FGameplayTag InputEventTag;

    // Combo state.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Combo")
    int32 MaxComboCount = 3;

    int32 ComboCount = 1;
    bool bSaveAttack = false;

    // Ability task callbacks.
    UFUNCTION()
    void OnMontageCompleted();
    
    UFUNCTION()
    void OnHitEventReceived(FGameplayEventData Payload);

    UFUNCTION()
    void OnComboEventReceived(FGameplayEventData Payload);
    
    // Buffers input while the ability is active.
    virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

    // Damage and hit tracing.
    UPROPERTY(EditDefaultsOnly, Category ="Damage")
    FScalableFloat Damage;
    void PerformMeleeTraceAndApplyDamage();
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
    TSubclassOf<class UGameplayEffect> DamageEffectClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
    float TraceDistance = 150.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Trace")
    float TraceRadius = 45.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat|Timing")
    float HitTraceDelay = 0.5f;

    FTimerHandle HitTraceTimerHandle;
    bool bTraceAlreadyFired = false;

    UFUNCTION()
    void OnHitTraceTimerFired();
};
