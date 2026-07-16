#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h" 
#include "PlayerAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

USTRUCT()
struct FEffectProperties
{
    GENERATED_BODY()
    FEffectProperties(){}
    
    FGameplayEffectContextHandle EffectContextHandle;
    
    UPROPERTY()
    UAbilitySystemComponent *SourceASC = nullptr;
    
    UPROPERTY()
    AActor * SourceAvatarActor = nullptr;
    
    UPROPERTY()
    AController* SourceController = nullptr;
    
    UPROPERTY()
    ACharacter * SourceCharacter= nullptr;
    
    UPROPERTY()
    UAbilitySystemComponent *TargetASC = nullptr;
    
    UPROPERTY()
    AActor * TargetAvatarActor = nullptr;
    
    UPROPERTY()
    AController* TargetController = nullptr;
    
    UPROPERTY()
    ACharacter * TargetCharacter = nullptr;
};

/**
 * Replicated player attributes used by GAS and UI.
 */
UCLASS()
class DEMO_API UPlayerAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
    
public: 
    UPlayerAttributeSet();

    // ==========================================================
    // Maps gameplay tags to attributes for data-driven UI lookup.
    // ==========================================================
    template<class T>
    using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

    TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;
    // ==========================================================


    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
    
    /*
     * ==========================================================
     * Vital attributes
     * ==========================================================
     */

    // Current health.
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Health);

    // Maximum health.
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHealth);

    
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireResistance, Category = "Resistance Attributes")
    FGameplayAttributeData FireResistance;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, FireResistance);
    
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IceResistance, Category = "Resistance Attributes")
    FGameplayAttributeData IceResistance;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, IceResistance);
    
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalResistance, Category = "Resistance Attributes")
    FGameplayAttributeData PhysicalResistance;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, PhysicalResistance);
    
    
    /*
     * ==========================================================
     * Primary attributes
     * ==========================================================
     */
    
    // Base attack value.
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attack, Category = "Primary Attributes")
    FGameplayAttributeData Attack;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Attack);

    // Base defense value.
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Defense, Category = "Primary Attributes")
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Defense);

    /*
     * ==========================================================
     * Secondary attributes
     * ==========================================================
     */

    // Critical-hit probability in the [0, 1] range.
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritRate, Category = "Secondary Attributes")
    FGameplayAttributeData CritRate;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CritRate);

    // Additional critical-hit damage expressed as a percentage.
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritDamage, Category = "Secondary Attributes")
    FGameplayAttributeData CritDamage;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CritDamage);

    // Energy regeneration multiplier.
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EnergyRegen, Category = "Secondary Attributes")
    FGameplayAttributeData EnergyRegen;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, EnergyRegen);

    /*
     * ==========================================================
     * Damage-bonus attributes
     * ==========================================================
     */

    // Skill damage bonus in the [0, 1] range.
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SkillDamageBonus, Category = "DamageBonus Attributes")
    FGameplayAttributeData SkillDamageBonus;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillDamageBonus);


    
    UPROPERTY(BlueprintReadOnly,Category="Meta Attributes")
    FGameplayAttributeData IncomingDamage;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, IncomingDamage);
    
    
    /*
     * ==========================================================
     * Replication notifications
     * ==========================================================
     */
    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
    
    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
    
    UFUNCTION()
    void OnRep_Attack(const FGameplayAttributeData& OldAttack) const;
    
    UFUNCTION()
    void OnRep_Defense(const FGameplayAttributeData& OldDefense) const;
    
    UFUNCTION()
    void OnRep_CritRate(const FGameplayAttributeData& OldCritRate) const;
    
    UFUNCTION()
    void OnRep_CritDamage(const FGameplayAttributeData& OldCritDamage) const;

    UFUNCTION()
    void OnRep_EnergyRegen(const FGameplayAttributeData& OldEnergyRegen) const;

    UFUNCTION()
    void OnRep_SkillDamageBonus(const FGameplayAttributeData& OldSkillDamageBonus) const;
    
    UFUNCTION()
    void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const;
    
    UFUNCTION()
    void OnRep_IceResistance(const FGameplayAttributeData& OldIceResistance) const;
    
    UFUNCTION()
    void OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const;
    

private:
    // Extracts source and target actors from an effect callback.
    void SetEffectProperties(const struct FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
    
    void ShowFloatingText(const FEffectProperties & Props , float Damage,bool bCriticalHit)const;
};
