// Rylan


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "Chaos/Deformable/MuscleActivationConstraints.h"


void UOverlayWidgetController::BroadcastInitialValues()
{
	const UPlayerAttributeSet* PlayerAttributeSet = CastChecked<UPlayerAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(PlayerAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(PlayerAttributeSet->GetMaxHealth());
	
	
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UPlayerAttributeSet* PlayerAttributeSet = CastChecked<UPlayerAttributeSet>(AttributeSet);
	

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetHealthAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
	{
		OnHealthChanged.Broadcast(Data.NewValue);	
	}	
	);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetMaxHealthAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
{
	OnHealthChanged.Broadcast(Data.NewValue);	
}	
);
	
	Cast<UPlayerAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
	[this](const FGameplayTagContainer& AssetTag)
	{
		for (const FGameplayTag& Tag : AssetTag)
		{     
			FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
			if (Tag.MatchesTag(MessageTag))
			{
				const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable,Tag);
				MessageWidgetRowDelegate.Broadcast(*Row);
			}
			
			
		
			
		}
	}	
	);
	
}

