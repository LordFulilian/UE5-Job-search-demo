#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExpComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnExperienceChangedSignature, int32, CurrentXP, int32, XPToNext);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeveledUpSignature, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnXPThresholdReachedSignature);

class UCurveTable;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class DEMO_API UExpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Experience")
	TObjectPtr<UCurveTable> ExperienceCurveTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int32 CurrentXP = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int32 CurrentLevel = 1;

	UPROPERTY(BlueprintAssignable)
	FOnExperienceChangedSignature OnExperienceChanged;

	UPROPERTY(BlueprintAssignable)
	FOnLeveledUpSignature OnLeveledUp;

	UPROPERTY(BlueprintAssignable)
	FOnXPThresholdReachedSignature OnXPThresholdReached;

	// Only accumulates XP. Does NOT auto-level.
	UFUNCTION(BlueprintCallable)
	void AddExperience(int32 Amount);

	// Returns true if player has enough XP to level up
	UFUNCTION(BlueprintPure)
	bool CanLevelUp() const;

	// Consumes required XP and increases level by 1. Call from "Level Up" button.
	UFUNCTION(BlueprintCallable)
	void TryLevelUp();

	UFUNCTION(BlueprintPure)
	int32 GetXPToNextLevel() const;

	UFUNCTION(BlueprintPure)
	float GetLevelProgress() const;
};