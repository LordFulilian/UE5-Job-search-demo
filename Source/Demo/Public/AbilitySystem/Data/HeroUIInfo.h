// Rylan

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "HeroUIInfo.generated.h"

/**
 * UI data for one party member.
 */
USTRUCT(BlueprintType)
struct FHeroSlotInfo
{
	GENERATED_BODY()

	// Stable identity tag, such as Hero.Shinbi.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag HeroTag = FGameplayTag();

	// Display name.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText HeroName = FText();

	// Portrait texture.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> AvatarIcon = nullptr;

	// Party-switch hotkey displayed next to the portrait.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText SwapHotkey = FText();
};

/**
 * Data asset describing the current party.
 */
UCLASS()
class DEMO_API UHeroUIInfo : public UDataAsset
{
	GENERATED_BODY()
    
public:
	// Party members in display order.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hero Information")
	TArray<FHeroSlotInfo> PartyMembers;
};
