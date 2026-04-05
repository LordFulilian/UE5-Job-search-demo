// Rylan


#include "input/PlayerInputConfig.h"

const UInputAction* UPlayerInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,bool boolNotFound) const
{
	for (const FPlayerInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}
	
	if (boolNotFound)
	{
		UE_LOG(LogTemp,Error,TEXT("Can't find AbilityInputAction for InputTag [%s]"),*InputTag.ToString(),*GetNameSafe(this));
	}
	return nullptr;
}
