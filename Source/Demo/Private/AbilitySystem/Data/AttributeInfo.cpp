// Rylan

#include "AbilitySystem/Data/AttributeInfo.h"

FAttributeInfo UAttributeInfoAsset::FindAttributeInfoForTag(const FGameplayTag& AttributesTag, bool bLogNotFound) const // 🌟 这里同步修改
{
	for (const FAttributeInfo& Info : AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributesTag))
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."), *AttributesTag.ToString(), *GetNameSafe(this));
	}
	return FAttributeInfo();
}