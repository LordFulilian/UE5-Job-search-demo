// Rylan

#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

struct FPlayerGameplayTags
{
public:
	static const FPlayerGameplayTags&  Get() {return GameplayTags;}
	static void InitializeNativeGameplayTags();
	FGameplayTag Attributes_Vital_Health;
	FGameplayTag Attributes_Vital_MaxHealth;
	FGameplayTag Attributes_Primary_Attack;
	FGameplayTag Attributes_Primary_Defense;
	FGameplayTag Attributes_Secondary_CritRate;
	FGameplayTag Attributes_Secondary_CritDamage;
	FGameplayTag Attributes_Secondary_EnergyRegen;
	FGameplayTag Attributes_DamageBonus_SkillDamageBonus;

	//Input Tag
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;

	
private:
	static FPlayerGameplayTags GameplayTags;
};
