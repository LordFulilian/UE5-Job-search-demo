// Rylan


#include "UI/HUD/PlayerHUD.h"
#include "UI/Widget/PlayerUserWidget.h"
#include "AbilitySystemComponent.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* APlayerHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	else
	{
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
	}
	return OverlayWidgetController;
}

UAttributeMenuWidgetController* APlayerHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this,AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		AttributeMenuWidgetController->BindCallbacksToDependencies();
	}
	return AttributeMenuWidgetController;
}

void APlayerHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass,TEXT("Overlay Widget Class uninitialized,please fill out BP_PlayerHUD"));
	checkf(OverlayWidgetControllerClass,TEXT("Overlay Widget Controller Class uninitilized,please fill out BP_PlayerHUD"));
	
	if (!OverlayWidget)
	{
		OverlayWidget = CreateWidget<UPlayerUserWidget>(PC, OverlayWidgetClass);
	}
	
	const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
	
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
	
	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();

	if (!OverlayWidget->IsInViewport())
	{
		OverlayWidget->AddToViewport();
	}
}

