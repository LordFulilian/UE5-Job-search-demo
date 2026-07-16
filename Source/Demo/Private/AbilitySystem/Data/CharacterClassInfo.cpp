// Rylan

#include "AbilitySystem/Data/CharacterClassInfo.h"

// Return the defaults registered for a character class.
FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	return CharacterClassInformation.FindChecked(CharacterClass);
}
