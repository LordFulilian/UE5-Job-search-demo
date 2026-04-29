// Rylan

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextComponent.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void DamageText(float  Damage);
};
