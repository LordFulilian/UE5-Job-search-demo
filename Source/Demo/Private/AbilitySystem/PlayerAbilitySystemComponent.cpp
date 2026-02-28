// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/PlayerAbilitySystemComponent.h"

void UPlayerAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UPlayerAbilitySystemComponent::EffectApplied);
}

void UPlayerAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                  const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	for (const FGameplayTag& Tag : TagContainer)
	{
		//TODO: 将标签广播到小部件控制器
		const FString Msg = FString::Printf(TEXT("GE Tag: %s"),*Tag.ToString());
		GEngine->AddOnScreenDebugMessage(-1,8.f,FColor::Red,Msg);
	}
}
