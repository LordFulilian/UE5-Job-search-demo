// Rylan


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::CharacterClassDefaultInfo(ECharacterClass CharacterClass)
{
	return CharacterClassInfomation.FindChecked(CharacterClass);
}
