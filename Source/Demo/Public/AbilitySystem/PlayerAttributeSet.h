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
 * 玩家属性集 
 */
UCLASS()
class DEMO_API UPlayerAttributeSet : public UAttributeSet
{
    GENERATED_BODY()
    
public: 
    UPlayerAttributeSet();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
    
    /*
     * ==========================================================
     * 核心生存属性 (Vital Attributes)
     * ==========================================================
     */

    // 1. 当前生命值
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
    FGameplayAttributeData Health;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Health);

    // 2. 最大生命值 
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
    FGameplayAttributeData MaxHealth;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHealth);

    /*
     * ==========================================================
     * 一级属性：基础面板 (Primary Attributes)
     * ==========================================================
     */
    
    // 3. 攻击力 (Attack) - 白值基础攻击
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attack, Category = "Primary Attributes")
    FGameplayAttributeData Attack;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Attack);

    // 4. 防御力 (Defense)
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Defense, Category = "Primary Attributes")
    FGameplayAttributeData Defense;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Defense);

    /*
     * ==========================================================
     * 二级属性：进阶战斗面板 (Secondary Attributes)
     * ==========================================================
     */

    // 5. 暴击率 (Crit Rate) - 通常以百分比表示，比如 0.05 代表 5%
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritRate, Category = "Secondary Attributes")
    FGameplayAttributeData CritRate;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CritRate);

    // 6. 暴击伤害 (Crit Damage) - 通常初始值为 1.5 代表 150% 爆伤
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritDamage, Category = "Secondary Attributes")
    FGameplayAttributeData CritDamage;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CritDamage);

    // 7. 共鸣效率/充能效率 (Energy Regen) - 默认 1.0 代表 100% 充能获取
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EnergyRegen, Category = "Secondary Attributes")
    FGameplayAttributeData EnergyRegen;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, EnergyRegen);

    /*
     * ==========================================================
     * 三级属性：独立乘区与增伤 (Damage Bonus Attributes)
     * ==========================================================
     */

    // 8. 技能伤害加成 (Skill Damage Bonus) - 例如 E 技能伤害提升 15% (0.15)
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SkillDamageBonus, Category = "DamageBonus Attributes")
    FGameplayAttributeData SkillDamageBonus;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, SkillDamageBonus);


    
    UPROPERTY(BlueprintReadOnly,Category="Meta Attributes")
    FGameplayAttributeData IncomingDamage;
    ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, IncomingDamage);
    
    
    /*
     * ==========================================================
     * 网络同步回调函数 (Replication Notifies)
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
    

private:
    // 效果属性辅助结构提取函数
    void SetEffectProperties(const struct FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;
    
    void ShowFloatingText(const FEffectProperties & Props , float Damage)const;
};