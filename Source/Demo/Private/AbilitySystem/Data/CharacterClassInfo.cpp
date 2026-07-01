// Rylan

#include "AbilitySystem/Data/CharacterClassInfo.h"

// 🔴 修复 1：函数名改为 GetClassDefaultInfo
FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	// 🔴 修复 2：修正 Information 的拼写
	return CharacterClassInformation.FindChecked(CharacterClass);
}