// Rylan

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/PlayerWidgetController.h"
#include "OverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature,float,NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHealthChangedSignature,float,NewMaxHealth);

/**
 * 
 */
UCLASS(BlueprintType,Blueprintable)
class DEMO_API UOverlayWidgetController : public UPlayerWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	
	UPROPERTY(BlueprintAssignable,Category="GAS|Attempts")
	FOnHealthChangedSignature OnHealthChanged;	
	
	UPROPERTY(BlueprintAssignable,Category="GAS|Attempts")
	FOnMaxHealthChangedSignature OnMaxHealthChanged;	
};
