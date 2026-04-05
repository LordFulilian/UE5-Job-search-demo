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
				// 顺序：动作资产, 触发事件, 实例对象, 函数指针, 传递给函数的标签参数
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}
			if (ReleaseFunc)
			{
				// 顺序同上
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleaseFunc, Action.InputTag);
			}
			if (HeldFunc)
			{
				// 顺序同上
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
	}
}
