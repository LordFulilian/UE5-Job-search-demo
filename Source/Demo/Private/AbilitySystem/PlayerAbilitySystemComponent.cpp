// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/PlayerAbilitySystemComponent.h"

#include "PlayerGameplayTags.h"
#include "AbilitySystem/Abilities/PlayerGameplayAbility.h"

void UPlayerAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UPlayerAbilitySystemComponent::EffectApplied);
	
}

void UPlayerAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		 FGameplayAbilitySpec AbilitySpec =  FGameplayAbilitySpec(AbilityClass,1);
		 if (const UPlayerGameplayAbility* PlayerAbility = Cast<UPlayerGameplayAbility>(AbilitySpec.Ability))
		 {
			 AbilitySpec.DynamicAbilityTags.AddTag(PlayerAbility->StartupInputTag);
		 	GiveAbility(AbilitySpec);
		 }
		
		
	}
}

void UPlayerAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UPlayerAbilitySystemComponent::AbilityInputReleased(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
			
		}
	}
}

void UPlayerAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                  const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	
	EffectAssetTags.Broadcast(TagContainer);
	
	
}
