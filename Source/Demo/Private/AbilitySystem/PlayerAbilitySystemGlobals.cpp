// Rylan


#include "AbilitySystem/PlayerAbilitySystemGlobals.h"

#include "PlayerAbilityTypes.h"

FGameplayEffectContext* UPlayerAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FPlayerGamePlayEffectContext();
}
