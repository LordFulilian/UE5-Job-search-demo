// Rylan

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};
class UAnimMontage;
/**
 * 
 */
class DEMO_API ICombatInterface
{
	GENERATED_BODY()

	
	
public:
	virtual int32 GetPlayerLevel();
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetHitReactMontage();
	
	virtual  void Die() = 0;
};
