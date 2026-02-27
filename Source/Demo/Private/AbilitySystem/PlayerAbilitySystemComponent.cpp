// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/PlayerAbilitySystemComponent.h"

void UPlayerAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UPlayerAbilitySystemComponent::EffectApplied);
}

void UPlayerAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                  const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	GEngine->AddOnScreenDebugMessage(1,8.f,FColor::Green,FString("EffectApplied"));
}
