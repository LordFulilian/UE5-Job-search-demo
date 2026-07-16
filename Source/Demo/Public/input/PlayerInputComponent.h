// Rylan

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "PlayerInputConfig.h"
#include "PlayerInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UPlayerInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
	
public:
	template<class UserClass,typename PressedFuncType,typename ReleaseFuncType,typename HeldFuncType>
	void BindAbilityAction(const UPlayerInputConfig* InputConfig,UserClass* Object,PressedFuncType PressedFunc,ReleaseFuncType ReleaseFunc,HeldFuncType HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleaseFuncType, typename HeldFuncType>
void UPlayerInputComponent::BindAbilityAction(const UPlayerInputConfig* InputConfig, UserClass* Object,PressedFuncType PressedFunc, ReleaseFuncType ReleaseFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);
    
	for (const FPlayerInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				// Bind the action, trigger event, receiver, callback, and input tag.
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}
			if (ReleaseFunc)
			{
				// Use the same argument order for release callbacks.
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleaseFunc, Action.InputTag);
			}
			if (HeldFunc)
			{
				// Use the same argument order for held callbacks.
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
	}
}
