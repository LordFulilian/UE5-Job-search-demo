// Rylan


#include "PlayerGameplayTags.h"
#include "GameplayTagsManager.h"

FPlayerGameplayTags FPlayerGameplayTags::GameplayTags;

void FPlayerGameplayTags::InitializeNativeGameplayTags()
{
	// Vital attributes.
	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.Health"), FString(TEXT("当前生命值")));
        
	GameplayTags.Attributes_Vital_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.MaxHealth"), FString(TEXT("最大生命值")));

	// Primary attributes.
	GameplayTags.Attributes_Primary_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Attack"), FString(TEXT("基础攻击力")));
        
	GameplayTags.Attributes_Primary_Defense = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Defense"), FString(TEXT("基础防御力")));

	// Secondary attributes.
	GameplayTags.Attributes_Secondary_CritRate = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CritRate"), FString(TEXT("暴击率")));
        
	GameplayTags.Attributes_Secondary_CritDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CritDamage"), FString(TEXT("暴击伤害")));
        
	GameplayTags.Attributes_Secondary_EnergyRegen = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.EnergyRegen"), FString(TEXT("共鸣/充能效率")));

	// Independent damage multipliers.
	GameplayTags.Attributes_DamageBonus_SkillDamageBonus = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.DamageBonus.SkillDamageBonus"), FString(TEXT("技能伤害加成")));

	
	//Input
	GameplayTags.InputTag_LMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag_LMB"), FString(TEXT("鼠标左键")));
	GameplayTags.InputTag_RMB = UGameplayTagsManager::Get().AddNativeGameplayTag(
			FName("InputTag_RMB"), FString(TEXT("鼠标右键")));
	GameplayTags.InputTag_1 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag_1"), FString(TEXT("数字1")));
	GameplayTags.InputTag_2 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag_2"), FString(TEXT("数字2")));
	GameplayTags.InputTag_3 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag_3"), FString(TEXT("数字3")));
	GameplayTags.InputTag_4 = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("InputTag_4"), FString(TEXT("数字4")));
	
	GameplayTags.Damage = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage"), FString(TEXT("Damage")));
	
	GameplayTags.Attributes_PlayerLevel = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.PlayerLevel"), FString(TEXT("玩家等级 (仅用于UI展示)")));
	
	GameplayTags.Damage_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Fire"), FString(TEXT("Fire Damage Type")));
	GameplayTags.Damage_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Physical"), FString(TEXT("Physical Damage Type")));
	GameplayTags.Damage_Ice = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Damage.Ice"), FString(TEXT("Ice Damage Type")));
	
	//Resistance
	GameplayTags.Attributes_Resistance_Fire = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Resistance.Fire"), FString(TEXT("Resistance to Fire damage")));
	GameplayTags.Attributes_Resistance_Physical = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Resistance.Physical"), FString(TEXT("Resistance to Physical damage")));
	GameplayTags.Attributes_Resistance_Ice = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Attributes.Resistance.Ice"), FString(TEXT("Resistance to Ice damage")));
	
	
	//Map fo Damage Types to Resistances
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Fire,GameplayTags.Attributes_Resistance_Fire);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Physical,GameplayTags.Attributes_Resistance_Physical);
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage, GameplayTags.Attributes_Resistance_Physical); // Fallback for generic Damage tag
	GameplayTags.DamageTypesToResistances.Add(GameplayTags.Damage_Ice,GameplayTags.Attributes_Resistance_Ice);
	
	
	GameplayTags.Effects_Hit_react = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Effects.Hit_react"), FString(TEXT("Effects_Hit_react")));

	GameplayTags.State_Invulnerable = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("State.Invulnerable"), FString(TEXT("Ignores incoming damage while present")));
	
	GameplayTags.Abilities_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
	FName("Abilities_Attack"), FString(TEXT("Abilities_Attack")));
}
