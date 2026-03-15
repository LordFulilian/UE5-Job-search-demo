// Rylan


#include "PlayerGameplayTags.h"
#include "GameplayTagsManager.h"

FPlayerGameplayTags FPlayerGameplayTags::GameplayTags;

void FPlayerGameplayTags::InitializeNativeGameplayTags()
{
	// 生存属性
	GameplayTags.Attributes_Vital_Health = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.Health"), FString(TEXT("当前生命值")));
        
	GameplayTags.Attributes_Vital_MaxHealth = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Vital.MaxHealth"), FString(TEXT("最大生命值")));

	// 一级属性
	GameplayTags.Attributes_Primary_Attack = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Attack"), FString(TEXT("基础攻击力")));
        
	GameplayTags.Attributes_Primary_Defense = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Primary.Defense"), FString(TEXT("基础防御力")));

	// 二级属性
	GameplayTags.Attributes_Secondary_CritRate = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CritRate"), FString(TEXT("暴击率")));
        
	GameplayTags.Attributes_Secondary_CritDamage = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.CritDamage"), FString(TEXT("暴击伤害")));
        
	GameplayTags.Attributes_Secondary_EnergyRegen = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.Secondary.EnergyRegen"), FString(TEXT("共鸣/充能效率")));

	// 独立乘区
	GameplayTags.Attributes_DamageBonus_SkillDamageBonus = UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Attributes.DamageBonus.SkillDamageBonus"), FString(TEXT("技能伤害加成")));
}
