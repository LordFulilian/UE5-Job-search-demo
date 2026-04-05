// Rylan

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "PlayerInputConfig.generated.h"


USTRUCT(BlueprintType)
struct FPlayerInputAction
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	const class UInputAction* InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly)
	 FGameplayTag InputTag = FGameplayTag();
};
/**
 * 
 */
UCLASS()
class DEMO_API UPlayerInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const UInputAction*FindAbilityInputActionForTag(const FGameplayTag& InputTag,bool boolNotFound = false ) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FPlayerInputAction> AbilityInputActions;
};
