// Rylan


#include "PlayerGameplayTags.h"
#include "GameplayTagsManager.h"

FPlayerGameplayTags FPlayerGameplayTags::GameplayTags;

void FPlayerGameplayTags::InitializeNativeGameplayTags()
{
	UGameplayTagsManager::Get().AddNativeGameplayTag(FName("GameplayTags"));
}
