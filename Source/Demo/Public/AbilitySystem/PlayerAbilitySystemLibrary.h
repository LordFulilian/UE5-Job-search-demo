// Rylan

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "PlayerAbilitySystemLibrary.generated.h"

class UAttributeMenuWidgetController;
/**
 * 
 */
UCLASS()
class DEMO_API UPlayerAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintPure,Category="PlayerAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController *GetOverlayWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure,Category="PlayerAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController *GetAttributeMenuWidgetController(const UObject* WorldContextObject);	
};
