// Rylan

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterClassInfo.h"
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

public:
	UFUNCTION(BlueprintPure,Category="PlayerAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController *GetOverlayWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure,Category="PlayerAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController *GetAttributeMenuWidgetController(const UObject* WorldContextObject);	
	
	UFUNCTION(BlueprintCallable,Category="PlayerAbilitySystemLibrary|CharacterClassDefaults")
	static  void InitialzeDefaultAttributes(const UObject* WorldContextObject,ECharacterClass CharacterClass ,float Level,UAbilitySystemComponent*ASC);
	
	UFUNCTION(BlueprintCallable,Category="PlayerAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject,UAbilitySystemComponent*ASC );

	UFUNCTION(BlueprintCallable,Category="PlayerAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable,Category="PlayerAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle EffectContextHandle);
	
	UFUNCTION(BlueprintCallable,Category="PlayerAbilitySystemLibrary|GameplayEffects")
	static  void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle EffectContextHandle,bool bInIsCriticalHit);
};
