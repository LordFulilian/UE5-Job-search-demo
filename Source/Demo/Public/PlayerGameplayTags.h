// Rylan

#pragma once
#include "PlayerGameplayTags.h"
#include "GameplayTagContainer.h"

struct FPlayerGameplayTags
{
public:
	static const FPlayerGameplayTags&  Get() {return GameplayTags;}
	static void InitializeNativeGameplayTags();
protected:
	
private:
	static FPlayerGameplayTags GameplayTags;
};
