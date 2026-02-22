// Rylan


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/PlayerAttributeSet.h"


void UOverlayWidgetController::BroadcastInitialValues()
{
	const UPlayerAttributeSet* PlayerAttributeSet = CastChecked<UPlayerAttributeSet>(AttributeSet);
	
	OnHealthChanged.Broadcast(PlayerAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(PlayerAttributeSet->GetMaxHealth());
	
	
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UPlayerAttributeSet* PlayerAttributeSet = CastChecked<UPlayerAttributeSet>(AttributeSet);
	

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetHealthAttribute()).AddUObject(this,&UOverlayWidgetController::HealthChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAttributeSet->GetMaxHealthAttribute()).AddUObject(this,&UOverlayWidgetController::MaxHealthChanged);
	
}

void UOverlayWidgetController::HealthChanged(const FOnAttributeChangeData& Data)const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UOverlayWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data)const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}
