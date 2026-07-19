// Rylan

#include "AbilitySystem/Data/AbilityInfo.h"
#include "Abilities/GameplayAbility.h"
#include "Demo/Demo.h"

FPlayerAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FPlayerAbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityTag.MatchesTagExact(AbilityTag))
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Info for AbilityTag [%s] on AbilityInfo [%s]."),
			*AbilityTag.ToString(), *GetNameSafe(this));
	}
	return FPlayerAbilityInfo();
}

FPlayerAbilityInfo UAbilityInfo::FindAbilityInfoForClass(
	TSubclassOf<UGameplayAbility> AbilityClass) const
{
	if (!AbilityClass) return FPlayerAbilityInfo();

	for (const FPlayerAbilityInfo& Info : AbilityInformation)
	{
		if (Info.AbilityClass == AbilityClass)
		{
			return Info;
		}
	}
	return FPlayerAbilityInfo();
}
