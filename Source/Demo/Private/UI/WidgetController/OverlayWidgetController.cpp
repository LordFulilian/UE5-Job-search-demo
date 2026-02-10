// Rylan


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/PlayerAttributeSet.h"


void UOverlayWidgetController::BroadcastInitialValues()
{
	const UPlayerAttributeSet* PlayerAttributeSet = CastChecked<UPlayerAttributeSet>(AttributeSet);
	
	UE_LOG(LogTemp, Warning, TEXT("BroadcastInitialValues - Health: %f, MaxHealth: %f"), 
		PlayerAttributeSet->GetHealth(), 
		PlayerAttributeSet->GetMaxHealth());
	OnHealthChanged.Broadcast(PlayerAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(PlayerAttributeSet->GetMaxHealth());
	
	
}
