#include "Components/QuestComponent.h"

#include "Components/PartyComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Quest/QuestDataAsset.h"

UQuestComponent::UQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

bool UQuestComponent::AcceptQuest(UQuestDataAsset* Quest)
{
	if (!IsValid(Quest) || Quest->QuestId.IsNone() || HasQuest(Quest->QuestId))
	{
		return false;
	}

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		ServerAcceptQuest(Quest);
		return true;
	}

	return AcceptQuestInternal(Quest);
}

bool UQuestComponent::AcceptQuestInternal(UQuestDataAsset* Quest)
{
	if (!IsValid(Quest) || Quest->QuestId.IsNone() || HasQuest(Quest->QuestId))
	{
		return false;
	}

	FQuestRuntimeEntry& NewEntry = QuestEntries.AddDefaulted_GetRef();
	NewEntry.Quest = Quest;
	if (TrackedQuestId.IsNone())
	{
		TrackedQuestId = Quest->QuestId;
		OnTrackedQuestChanged.Broadcast();
	}
	OnQuestAccepted.Broadcast(Quest);
	OnQuestListChanged.Broadcast();
	if (AActor* Owner = GetOwner()) Owner->ForceNetUpdate();

	return true;
}

bool UQuestComponent::NotifyObjectiveProgress(
	FName ObjectiveTargetId, int32 ProgressAmount)
{
	if (ObjectiveTargetId.IsNone() || ProgressAmount <= 0) return false;

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		ServerNotifyObjectiveProgress(ObjectiveTargetId, ProgressAmount);
		return true;
	}

	return NotifyObjectiveProgressInternal(ObjectiveTargetId, ProgressAmount);
}

bool UQuestComponent::NotifyObjectiveProgressInternal(
	FName ObjectiveTargetId, int32 ProgressAmount)
{
	bool bChanged = false;
	for (FQuestRuntimeEntry& Entry : QuestEntries)
	{
		if (!IsValid(Entry.Quest) ||
			Entry.State != EQuestRuntimeState::Active ||
			!Entry.Quest->MatchesObjectiveTarget(ObjectiveTargetId))
		{
			continue;
		}

		const int32 RequiredCount = FMath::Max(1, Entry.Quest->RequiredCount);
		Entry.CurrentProgress = FMath::Clamp(
			Entry.CurrentProgress + ProgressAmount, 0, RequiredCount);
		OnQuestProgressChanged.Broadcast(
			Entry.Quest, Entry.CurrentProgress, RequiredCount);
		bChanged = true;

		if (Entry.CurrentProgress >= RequiredCount)
		{
			Entry.State = EQuestRuntimeState::Completed;
			OnQuestCompleted.Broadcast(Entry.Quest);
			GrantReward(Entry);

		}
	}

	if (bChanged)
	{
		SelectNextTrackedQuest();
		OnQuestListChanged.Broadcast();
		if (AActor* Owner = GetOwner()) Owner->ForceNetUpdate();
	}
	return bChanged;
}

bool UQuestComponent::ClaimQuestReward(FName QuestId)
{
	const int32 QuestIndex = FindQuestIndex(QuestId);
	if (!QuestEntries.IsValidIndex(QuestIndex) ||
		QuestEntries[QuestIndex].State != EQuestRuntimeState::Completed ||
		QuestEntries[QuestIndex].bRewardClaimed)
	{
		return false;
	}

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		ServerClaimQuestReward(QuestId);
		return true;
	}

	return ClaimQuestRewardInternal(QuestId);
}

bool UQuestComponent::ClaimQuestRewardInternal(FName QuestId)
{
	const int32 QuestIndex = FindQuestIndex(QuestId);
	if (!QuestEntries.IsValidIndex(QuestIndex)) return false;

	const bool bClaimed = GrantReward(QuestEntries[QuestIndex]);
	if (bClaimed)
	{
		OnQuestListChanged.Broadcast();
		if (AActor* Owner = GetOwner()) Owner->ForceNetUpdate();
	}
	return bClaimed;
}

bool UQuestComponent::GrantReward(FQuestRuntimeEntry& QuestEntry)
{
	if (!IsValid(QuestEntry.Quest) ||
		QuestEntry.State != EQuestRuntimeState::Completed ||
		QuestEntry.bRewardClaimed)
	{
		return false;
	}

	if (QuestEntry.Quest->RewardHeroTag.IsValid())
	{
		AActor* Owner = GetOwner();
		if (UPartyComponent* PartyComponent = Owner
			? Owner->FindComponentByClass<UPartyComponent>()
			: nullptr)
		{
			PartyComponent->UnlockHero(QuestEntry.Quest->RewardHeroTag);
		}
		else
		{
			return false;
		}
	}

	QuestEntry.bRewardClaimed = true;
	OnQuestRewardClaimed.Broadcast(QuestEntry.Quest);
	return true;
}

bool UQuestComponent::HasQuest(FName QuestId) const
{
	return FindQuestIndex(QuestId) != INDEX_NONE;
}

bool UQuestComponent::IsQuestCompleted(FName QuestId) const
{
	const int32 QuestIndex = FindQuestIndex(QuestId);
	return QuestEntries.IsValidIndex(QuestIndex) &&
		QuestEntries[QuestIndex].State == EQuestRuntimeState::Completed;
}

bool UQuestComponent::HasActiveObjective(FName ObjectiveTargetId) const
{
	if (ObjectiveTargetId.IsNone()) return false;

	return QuestEntries.ContainsByPredicate(
		[ObjectiveTargetId](const FQuestRuntimeEntry& Entry)
		{
			return IsValid(Entry.Quest) &&
				Entry.State == EQuestRuntimeState::Active &&
				Entry.Quest->MatchesObjectiveTarget(ObjectiveTargetId);
		});
}

bool UQuestComponent::GetQuestEntry(
	FName QuestId, FQuestRuntimeEntry& OutEntry) const
{
	const int32 QuestIndex = FindQuestIndex(QuestId);
	if (!QuestEntries.IsValidIndex(QuestIndex)) return false;
	OutEntry = QuestEntries[QuestIndex];
	return true;
}

bool UQuestComponent::SetTrackedQuest(FName QuestId)
{
	if (!QuestId.IsNone())
	{
		const int32 QuestIndex = FindQuestIndex(QuestId);
		if (!QuestEntries.IsValidIndex(QuestIndex) ||
			QuestEntries[QuestIndex].State != EQuestRuntimeState::Active)
		{
			return false;
		}
	}

	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		ServerSetTrackedQuest(QuestId);
		return true;
	}

	return SetTrackedQuestInternal(QuestId);
}

bool UQuestComponent::SetTrackedQuestInternal(FName QuestId)
{
	if (TrackedQuestId == QuestId) return false;
	if (!QuestId.IsNone())
	{
		const int32 QuestIndex = FindQuestIndex(QuestId);
		if (!QuestEntries.IsValidIndex(QuestIndex) ||
			QuestEntries[QuestIndex].State != EQuestRuntimeState::Active)
		{
			return false;
		}
	}

	TrackedQuestId = QuestId;
	OnTrackedQuestChanged.Broadcast();
	OnQuestListChanged.Broadcast();
	if (AActor* Owner = GetOwner()) Owner->ForceNetUpdate();
	return true;
}

bool UQuestComponent::GetTrackedQuest(FQuestRuntimeEntry& OutEntry) const
{
	const int32 QuestIndex = FindQuestIndex(TrackedQuestId);
	if (!QuestEntries.IsValidIndex(QuestIndex) ||
		QuestEntries[QuestIndex].State != EQuestRuntimeState::Active)
	{
		return false;
	}

	OutEntry = QuestEntries[QuestIndex];
	return true;
}

void UQuestComponent::SelectNextTrackedQuest()
{
	const int32 TrackedIndex = FindQuestIndex(TrackedQuestId);
	if (QuestEntries.IsValidIndex(TrackedIndex) &&
		QuestEntries[TrackedIndex].State == EQuestRuntimeState::Active)
	{
		return;
	}

	FName NextQuestId = NAME_None;
	for (const FQuestRuntimeEntry& Entry : QuestEntries)
	{
		if (IsValid(Entry.Quest) &&
			Entry.State == EQuestRuntimeState::Active)
		{
			NextQuestId = Entry.Quest->QuestId;
			break;
		}
	}

	if (TrackedQuestId != NextQuestId)
	{
		TrackedQuestId = NextQuestId;
		OnTrackedQuestChanged.Broadcast();
	}
}

int32 UQuestComponent::FindQuestIndex(FName QuestId) const
{
	if (QuestId.IsNone()) return INDEX_NONE;
	return QuestEntries.IndexOfByPredicate(
		[QuestId](const FQuestRuntimeEntry& Entry)
		{
			return IsValid(Entry.Quest) && Entry.Quest->QuestId == QuestId;
		});
}

void UQuestComponent::ServerAcceptQuest_Implementation(UQuestDataAsset* Quest)
{
	AcceptQuestInternal(Quest);
}

void UQuestComponent::ServerNotifyObjectiveProgress_Implementation(
	FName ObjectiveTargetId, int32 ProgressAmount)
{
	NotifyObjectiveProgressInternal(ObjectiveTargetId, ProgressAmount);
}

void UQuestComponent::ServerClaimQuestReward_Implementation(FName QuestId)
{
	ClaimQuestRewardInternal(QuestId);
}

void UQuestComponent::ServerSetTrackedQuest_Implementation(FName QuestId)
{
	SetTrackedQuestInternal(QuestId);
}

void UQuestComponent::OnRep_QuestEntries(
	const TArray<FQuestRuntimeEntry>& PreviousEntries)
{
	for (const FQuestRuntimeEntry& Entry : QuestEntries)
	{
		if (!IsValid(Entry.Quest)) continue;

		const FQuestRuntimeEntry* PreviousEntry = PreviousEntries.FindByPredicate(
			[&Entry](const FQuestRuntimeEntry& Candidate)
			{
				return IsValid(Candidate.Quest) &&
					Candidate.Quest->QuestId == Entry.Quest->QuestId;
			});

		if (!PreviousEntry)
		{
			OnQuestAccepted.Broadcast(Entry.Quest);
		}
		else
		{
			if (PreviousEntry->CurrentProgress != Entry.CurrentProgress)
			{
				OnQuestProgressChanged.Broadcast(
					Entry.Quest, Entry.CurrentProgress,
					FMath::Max(1, Entry.Quest->RequiredCount));
			}
			if (PreviousEntry->State != EQuestRuntimeState::Completed &&
				Entry.State == EQuestRuntimeState::Completed)
			{
				OnQuestCompleted.Broadcast(Entry.Quest);
			}
			if (!PreviousEntry->bRewardClaimed && Entry.bRewardClaimed)
			{
				OnQuestRewardClaimed.Broadcast(Entry.Quest);
			}
		}
	}
	OnQuestListChanged.Broadcast();
}

void UQuestComponent::OnRep_TrackedQuestId()
{
	OnTrackedQuestChanged.Broadcast();
	OnQuestListChanged.Broadcast();
}

void UQuestComponent::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UQuestComponent, QuestEntries, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UQuestComponent, TrackedQuestId, COND_OwnerOnly);
}
