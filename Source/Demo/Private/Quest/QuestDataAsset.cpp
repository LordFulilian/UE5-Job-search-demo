// Rylan


#include "Quest/QuestDataAsset.h"

bool UQuestDataAsset::MatchesObjectiveTarget(FName ReportedTargetId) const
{
	if (ReportedTargetId.IsNone()) return false;
	if (!ObjectiveTargetId.IsNone())
	{
		return ObjectiveTargetId == ReportedTargetId;
	}
	if (QuestId.IsNone()) return false;
	if (QuestId == ReportedTargetId) return true;

	FString LegacyObjectiveId = QuestId.ToString();
	LegacyObjectiveId.ReplaceInline(TEXT("_"), TEXT("."));
	return FName(*LegacyObjectiveId) == ReportedTargetId;
}

