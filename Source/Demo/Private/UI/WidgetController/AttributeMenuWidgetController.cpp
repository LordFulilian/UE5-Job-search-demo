// Rylan

#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "PlayerGameplayTags.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/OPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
    // Bind dependencies owned by the base controller first.
    Super::BindCallbacksToDependencies();
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
    // Broadcast shared values such as player level first.
    Super::BroadcastInitialValues();
    
    UPlayerAttributeSet *AS = CastChecked<UPlayerAttributeSet>(AttributeSet);
    
    check(AttributeInfo)
    
    if (AOPlayerState* OPlayerState = Cast<AOPlayerState>(PlayerState))
    {
        // Resolve display metadata from the configured data asset.
        FPlayerAttributeInfo LevelInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_PlayerLevel);
        
        // Attach the current numeric value.
        LevelInfo.AttributeValue = OPlayerState->GetPlayerLevel();
        
        // Broadcast the completed attribute payload.
        AttributeInfoDelegate.Broadcast(LevelInfo);
    }
    
    /* Vital attributes. */
    
    // Health.
    FPlayerAttributeInfo HealthInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Vital_Health);
    HealthInfo.AttributeValue = AS->GetHealth();
    AttributeInfoDelegate.Broadcast(HealthInfo);

    // Maximum health.
    FPlayerAttributeInfo MaxHealthInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Vital_MaxHealth);
    MaxHealthInfo.AttributeValue = AS->GetMaxHealth();
    AttributeInfoDelegate.Broadcast(MaxHealthInfo);

    /* Primary attributes. */

    // Attack.
    FPlayerAttributeInfo AttackInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Primary_Attack);
    AttackInfo.AttributeValue = AS->GetAttack();
    AttributeInfoDelegate.Broadcast(AttackInfo);

    // Defense.
    FPlayerAttributeInfo DefenseInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Primary_Defense);
    DefenseInfo.AttributeValue = AS->GetDefense();
    AttributeInfoDelegate.Broadcast(DefenseInfo);

    /* Secondary attributes. */

    // Critical-hit rate.
    FPlayerAttributeInfo CritRateInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Secondary_CritRate);
    CritRateInfo.AttributeValue = AS->GetCritRate();
    AttributeInfoDelegate.Broadcast(CritRateInfo);

    // Critical-hit damage.
    FPlayerAttributeInfo CritDamageInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Secondary_CritDamage);
    CritDamageInfo.AttributeValue = AS->GetCritDamage();
    AttributeInfoDelegate.Broadcast(CritDamageInfo);

    // Energy regeneration.
    FPlayerAttributeInfo EnergyRegenInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Secondary_EnergyRegen);
    EnergyRegenInfo.AttributeValue = AS->GetEnergyRegen();
    AttributeInfoDelegate.Broadcast(EnergyRegenInfo);

    /* Damage-bonus attributes. */

    // Skill damage bonus.
    FPlayerAttributeInfo SkillDamageBonusInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_DamageBonus_SkillDamageBonus);
    SkillDamageBonusInfo.AttributeValue = AS->GetSkillDamageBonus();
    AttributeInfoDelegate.Broadcast(SkillDamageBonusInfo);
    
    /* Resistance attributes. */

    // Fire resistance.
    FPlayerAttributeInfo FireResistInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Resistance_Fire);
    FireResistInfo.AttributeValue = AS->GetFireResistance();
    AttributeInfoDelegate.Broadcast(FireResistInfo);

    // Ice resistance.
    FPlayerAttributeInfo IceResistInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Resistance_Ice);
    IceResistInfo.AttributeValue = AS->GetIceResistance();
    AttributeInfoDelegate.Broadcast(IceResistInfo);

    // Physical resistance.
    FPlayerAttributeInfo PhysicalResistInfo = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Resistance_Physical);
    PhysicalResistInfo.AttributeValue = AS->GetPhysicalResistance();
    AttributeInfoDelegate.Broadcast(PhysicalResistInfo);
}
