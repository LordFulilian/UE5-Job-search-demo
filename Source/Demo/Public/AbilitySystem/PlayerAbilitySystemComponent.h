// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PlayerAbilitySystemComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags,const FGameplayTagContainer& );
/**
 * 
 */
UCLASS()
class DEMO_API UPlayerAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void AbilityActorInfoSet();
	
	FEffectAssetTags EffectAssetTags;
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);	

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputReleased(const FGameplayTag& InputTag);
protected:
	
	void EffectApplied(UAbilitySystemComponent *AbilitySystemComponent , const FGameplayEffectSpec &EffectSpec ,FActiveGameplayEffectHandle ActiveEffectHandle );
};
