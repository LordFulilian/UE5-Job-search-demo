// Rylan


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "PlayerGameplayTags.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"


void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	
	UPlayerAttributeSet	 *AS = CastChecked<UPlayerAttributeSet>(AttributeSet);
	
	check(AttributeInfo)
	FAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(FPlayerGameplayTags::Get().Attributes_Primary_Attack);
	Info.AttributeValue=  AS->GetAttack();
	AttributeInfoDelegate.Broadcast(Info);
}
