// Rylan

#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "PlayerGameplayTags.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/OPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
    // 🔴 养成好习惯：即使里面暂时没写东西，只要重写了父类虚函数，第一行永远先喊父类
    Super::BindCallbacksToDependencies();
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
    // 🔴 关键修复：先让基类把“等级(Level)”等全局通用数据发出去！
    Super::BroadcastInitialValues();
    
    UPlayerAttributeSet *AS = CastChecked<UPlayerAttributeSet>(AttributeSet);
    
    check(AttributeInfo)
    
    if (AOPlayerState* OPlayerState = Cast<AOPlayerState>(PlayerState))
    {
        // 1. 去你配好的 DataAsset 里，把“名字”和“标签”取出来
        FPlayerAttributeInfo LevelInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_PlayerLevel);
        
        // 2. 把“数值”塞进快递盒
        LevelInfo.AttributeValue = OPlayerState->GetPlayerLevel();
        
        // 3. 走通用属性频道，把完整的快递盒广播给 UI！
        AttributeInfoDelegate.Broadcast(LevelInfo);
    }
    
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
    FPlayerAttributeInfo SkillDamageBonusInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_DamageBonus_SkillDamageBonus);
    SkillDamageBonusInfo.AttributeValue = AS->GetSkillDamageBonus();
    AttributeInfoDelegate.Broadcast(SkillDamageBonusInfo);
    
    /* --- Resistance Attributes (抗性属性) --- */

    // 9. Fire Resistance (火抗性)
    FPlayerAttributeInfo FireResistInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Resistance_Fire);
    FireResistInfo.AttributeValue = AS->GetFireResistance();
    AttributeInfoDelegate.Broadcast(FireResistInfo);

    // 10. Ice Resistance (冰抗性)
    FPlayerAttributeInfo IceResistInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Resistance_Ice);
    IceResistInfo.AttributeValue = AS->GetIceResistance();
    AttributeInfoDelegate.Broadcast(IceResistInfo);

    // 11. Physical Resistance (物理抗性)
    FPlayerAttributeInfo PhysicalResistInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Resistance_Physical);
    PhysicalResistInfo.AttributeValue = AS->GetPhysicalResistance();
    AttributeInfoDelegate.Broadcast(PhysicalResistInfo);
}