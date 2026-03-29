// Rylan


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "PlayerGameplayTags.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"


void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	
	UPlayerAttributeSet	 *AS = CastChecked<UPlayerAttributeSet>(AttributeSet);
	
	check(AttributeInfo)
	/* --- Vital Attributes (核心生存属性) --- */
    
    // 1. Health (当前生命值)
    FPlayerAttributeInfo HealthInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Vital_Health);
    HealthInfo.AttributeValue = AS->GetHealth();
    AttributeInfoDelegate.Broadcast(HealthInfo);

    // 2. MaxHealth (最大生命值)
    FPlayerAttributeInfo MaxHealthInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Vital_MaxHealth);
    MaxHealthInfo.AttributeValue = AS->GetMaxHealth();
    AttributeInfoDelegate.Broadcast(MaxHealthInfo);

    /* --- Primary Attributes (一级属性) --- */

    // 3. Attack (攻击力)
    FPlayerAttributeInfo AttackInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Primary_Attack);
    AttackInfo.AttributeValue = AS->GetAttack();
    AttributeInfoDelegate.Broadcast(AttackInfo);

    // 4. Defense (防御力)
    FPlayerAttributeInfo DefenseInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Primary_Defense);
    DefenseInfo.AttributeValue = AS->GetDefense();
    AttributeInfoDelegate.Broadcast(DefenseInfo);

    /* --- Secondary Attributes (二级属性) --- */

    // 5. CritRate (暴击率)
    FPlayerAttributeInfo CritRateInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Secondary_CritRate);
    CritRateInfo.AttributeValue = AS->GetCritRate();
    AttributeInfoDelegate.Broadcast(CritRateInfo);

    // 6. CritDamage (暴击伤害)
    FPlayerAttributeInfo CritDamageInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Secondary_CritDamage);
    CritDamageInfo.AttributeValue = AS->GetCritDamage();
    AttributeInfoDelegate.Broadcast(CritDamageInfo);

    // 7. EnergyRegen (共鸣效率/充能效率)
    FPlayerAttributeInfo EnergyRegenInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Secondary_EnergyRegen);
    EnergyRegenInfo.AttributeValue = AS->GetEnergyRegen();
    AttributeInfoDelegate.Broadcast(EnergyRegenInfo);

    /* --- Damage Bonus Attributes (三级属性) --- */

    // 8. SkillDamageBonus (技能伤害加成)
    // 注意：请确保你在 PlayerGameplayTags.h 中定义的标签名是 Attributes_DamageBonus_SkillDamageBonus
    FPlayerAttributeInfo SkillDamageBonusInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_DamageBonus_SkillDamageBonus);
    SkillDamageBonusInfo.AttributeValue = AS->GetSkillDamageBonus();
    AttributeInfoDelegate.Broadcast(SkillDamageBonusInfo);
	
}
