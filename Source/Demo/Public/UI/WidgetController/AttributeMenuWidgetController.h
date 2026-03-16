// Rylan

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/PlayerWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class DEMO_API UAttributeMenuWidgetController : public UPlayerWidgetController
{
	GENERATED_BODY()
	
	
public:
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;
};
