// Rylan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/Widget/PlayerUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "PlayerHUD.generated.h"

class UOverlayWidgetController;
class UPlayerUserWidget;
struct FWidgetControllerParmas;
/**
 * 
 */
UCLASS()
class DEMO_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	TObjectPtr<UPlayerUserWidget> OverlayWidget;
	
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParmas& WCParmas);
	
	void InitOverlay(APlayerController* PC,APlayerState* PS,UAbilitySystemComponent* ASC,UAttributeSet*AS);
protected:

	
private:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerUserWidget> OverlayWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;
};
