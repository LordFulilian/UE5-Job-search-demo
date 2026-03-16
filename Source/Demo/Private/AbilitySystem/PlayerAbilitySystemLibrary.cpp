// Rylan


#include "AbilitySystem/PlayerAbilitySystemLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/PlayerWidgetController.h"
#include "Player/OPlayerState.h"
#include "UI/HUD/PlayerHUD.h"

UOverlayWidgetController* UPlayerAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController *PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PC->GetHUD()))
		{
			AOPlayerState *PS = PC->GetPlayerState<AOPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
			return PlayerHUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UPlayerAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController *PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PC->GetHUD()))
		{
			AOPlayerState *PS = PC->GetPlayerState<AOPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
			return PlayerHUD->GetAttributeMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}
