// Rylan


#include "UI/HUD/PlayerHUD.h"
#include "UI/Widget/PlayerUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* APlayerHUD::GetOverlayWidgetController(const FWidgetControllerParmas& WCParmas)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParmas(WCParmas);
		
		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

void APlayerHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass,TEXT("Overlay Widget Class uninitialized,please fill out BP_PlayerHUD"));
	checkf(OverlayWidgetControllerClass,TEXT("Overlay Widget Controller Class uninitilized,please fill out BP_PlayerHUD"));
	
	UUserWidget*Widget= CreateWidget<UUserWidget>(GetWorld(),OverlayWidgetClass);
	OverlayWidget = Cast<UPlayerUserWidget>(Widget);
	
	const FWidgetControllerParmas WidgetControllerParmas(PC,PS,ASC,AS);
	
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParmas);
	
	OverlayWidget->SetWidgetController(WidgetController);
	
	Widget->AddToViewport();
	
	
}


