// Rylan

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "QuestDataAsset.generated.h"

UCLASS(BlueprintType)
class DEMO_API UQuestDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// Explicit ObjectiveTargetId is preferred. Legacy quest assets fall back
	// to their QuestId, with underscores treated as gameplay-ID separators.
	UFUNCTION(BlueprintPure, Category = "Quest|Objective")
	bool MatchesObjectiveTarget(FName ReportedTargetId) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FName QuestId = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	FText Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest",
		meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest",
		meta = (MultiLine = true))
	FText ObjectiveText;

	// Stable objective identity reported by gameplay actors, for example Boss.Aurora.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest|Objective")
	FName ObjectiveTargetId = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest|Objective",
		meta = (ClampMin = "1"))
	int32 RequiredCount = 1;

	// A completed quest automatically unlocks this hero.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest|Reward")
	FGameplayTag RewardHeroTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest|Reward")
	FText RewardText;
};
