// Rylan

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/PlayerWidgetController.h"
#include "AbilitySystem/Data/AttributeInfo.h" 
#include "AttributeMenuWidgetController.generated.h" 


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FPlayerAttributeInfo&, Info);

/**
 * */
UCLASS(BlueprintType, Blueprintable)
class DEMO_API UAttributeMenuWidgetController : public UPlayerWidgetController
{
	GENERATED_BODY()
    
public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;
    
	// UI Blueprints listen to this delegate to populate attribute entries.
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
    
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;
};
