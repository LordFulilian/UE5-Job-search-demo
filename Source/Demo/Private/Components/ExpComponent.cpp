#include "Components/ExpComponent.h"
#include "Engine/CurveTable.h"

void UExpComponent::AddExperience(int32 Amount)
{
	CurrentXP += Amount;

	// Check if threshold reached, broadcast for UI (button glow / enable)
	if (CanLevelUp())
	{
		OnXPThresholdReached.Broadcast();
	}

	OnExperienceChanged.Broadcast(CurrentXP, GetXPToNextLevel());
}

bool UExpComponent::CanLevelUp() const
{
	return CurrentXP >= GetXPToNextLevel();
}

void UExpComponent::TryLevelUp()
{
	if (!CanLevelUp()) return;

	int32 Required = GetXPToNextLevel();
	CurrentXP -= Required;
	CurrentLevel++;

	OnLeveledUp.Broadcast(CurrentLevel);

	// If still enough XP after level-up, broadcast threshold again
	if (CanLevelUp())
	{
		OnXPThresholdReached.Broadcast();
	}

	OnExperienceChanged.Broadcast(CurrentXP, GetXPToNextLevel());
}

int32 UExpComponent::GetXPToNextLevel() const
{
	if (!ExperienceCurveTable) return 999999;

	static const FName RowName = FName("XP");
	FRealCurve* Curve = ExperienceCurveTable->FindCurve(RowName, FString());
	if (Curve)
	{
		return FMath::RoundToInt(Curve->Eval(static_cast<float>(CurrentLevel)));
	}
	return CurrentLevel * 100 + CurrentLevel * CurrentLevel * 25;
}

float UExpComponent::GetLevelProgress() const
{
	int32 Required = GetXPToNextLevel();
	return Required > 0 ? (float)CurrentXP / (float)Required : 1.0f;
}