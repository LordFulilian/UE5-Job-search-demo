#include "Character/BossCharacter.h"

#include "AbilitySystem/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "AI/PlayerAIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/QuestComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "PlayerGameplayTags.h"
#include "TimerManager.h"
#include "Player/OnePlayerController.h"
#include "Player/OPlayerState.h"

ABossCharacter::ABossCharacter()
{
	BaseWalkSpeed = 0.f;

	BossEncounterTrigger =
		CreateDefaultSubobject<USphereComponent>(TEXT("BossEncounterTrigger"));
	BossEncounterTrigger->SetupAttachment(GetRootComponent());
	BossEncounterTrigger->InitSphereRadius(800.f);
	BossEncounterTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BossEncounterTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	BossEncounterTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	BossEncounterTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BossEncounterTrigger->SetCollisionResponseToChannel(
		ECC_GameTraceChannel1, ECR_Overlap);
	BossEncounterTrigger->SetGenerateOverlapEvents(true);
}

void ABossCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ApplyDormantState();
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	GrantBossAbilities(PhaseOneAbilities);
	OnHealthChanged.AddUniqueDynamic(this, &ABossCharacter::HandleHealthChanged);
	BossEncounterTrigger->OnComponentBeginOverlap.AddUniqueDynamic(
		this, &ABossCharacter::HandleEncounterBeginOverlap);
	BossEncounterTrigger->OnComponentEndOverlap.AddUniqueDynamic(
		this, &ABossCharacter::HandleEncounterEndOverlap);

	SetArenaBarriersEnabled(false);
	ApplyDormantState();
}

void ABossCharacter::HandleEncounterBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerCharacter ||
		OtherComponent != PlayerCharacter->GetCapsuleComponent() ||
		bIsDead || BossPhase == EBossPhase::Dead)
	{
		return;
	}

	if (HasAuthority())
	{
		ActivateBoss();
	}

	if (AOnePlayerController* PlayerController =
		Cast<AOnePlayerController>(PlayerCharacter->GetController()))
	{
		PlayerController->ShowBossHealthBar(this);
	}
}

void ABossCharacter::HandleEncounterEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerCharacter ||
		OtherComponent != PlayerCharacter->GetCapsuleComponent())
	{
		return;
	}

	if (AOnePlayerController* PlayerController =
		Cast<AOnePlayerController>(PlayerCharacter->GetController()))
	{
		PlayerController->LeaveBossEncounter(this);
	}
}

void ABossCharacter::ActivateBoss()
{
	if (!HasAuthority() || BossPhase != EBossPhase::Dormant || bIsDead) return;

	BaseWalkSpeed = PhaseOneWalkSpeed;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	SetBossPhase(EBossPhase::PhaseOne);
	SetBossInvulnerable(false);
	if (UWorld* World = GetWorld())
	{
		for (FConstPlayerControllerIterator It =
			World->GetPlayerControllerIterator(); It; ++It)
		{
			if (AOnePlayerController* PlayerController =
				Cast<AOnePlayerController>(It->Get()))
			{
				PlayerController->ShowBossHealthBar(this);
			}
		}
	}
	if (PlayerAIController && PlayerAIController->GetBrainComponent())
	{
		PlayerAIController->GetBrainComponent()->ResumeLogic(TEXT("Boss activated"));
		PlayerAIController->GetBrainComponent()->RestartLogic();
	}
}

void ABossCharacter::HandleHealthChanged(float NewHealth)
{
	if (!HasAuthority() || BossPhase != EBossPhase::PhaseOne ||
		!AttributeSet || NewHealth <= 0.f)
	{
		return;
	}

	const UPlayerAttributeSet* BossAttributes = Cast<UPlayerAttributeSet>(AttributeSet);
	if (BossAttributes && BossAttributes->GetMaxHealth() > 0.f &&
		NewHealth / BossAttributes->GetMaxHealth() <= PhaseTwoHealthPercent)
	{
		SetBossInvulnerable(true);
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->CancelAllAbilities();
		}
		if (PlayerAIController)
		{
			PlayerAIController->StopMovement();
		}
		GetCharacterMovement()->DisableMovement();
		if (PlayerAIController && PlayerAIController->GetBrainComponent())
		{
			PlayerAIController->GetBrainComponent()->PauseLogic(TEXT("Boss phase transition"));
		}
		SetBossPhase(EBossPhase::Transition);
		GetWorldTimerManager().SetTimer(
			PhaseTransitionTimer, this, &ABossCharacter::CompletePhaseTransition,
			PhaseTransitionDuration, false);
	}
}

void ABossCharacter::CompletePhaseTransition()
{
	if (!HasAuthority() || BossPhase != EBossPhase::Transition || bIsDead) return;
	GetWorldTimerManager().ClearTimer(PhaseTransitionTimer);

	if (!bPhaseTwoAbilitiesGranted)
	{
		GrantBossAbilities(PhaseTwoAbilities);
		bPhaseTwoAbilitiesGranted = true;
	}

	BaseWalkSpeed = PhaseTwoWalkSpeed;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	SetBossPhase(EBossPhase::PhaseTwo);
	SetBossInvulnerable(false);
	if (PlayerAIController && PlayerAIController->GetBrainComponent())
	{
		PlayerAIController->GetBrainComponent()->ResumeLogic(TEXT("Boss phase two"));
	}
}

void ABossCharacter::Die()
{
	if (bIsDead) return;

	if (HasAuthority() && !QuestObjectiveId.IsNone())
	{
		bool bProgressAwarded = false;
		auto TryAwardProgress = [this, &bProgressAwarded](AOPlayerState* Candidate)
		{
			if (bProgressAwarded || !IsValid(Candidate)) return;
			if (UQuestComponent* QuestComponent = Candidate->GetQuestComponent())
			{
				if (QuestComponent->HasActiveObjective(QuestObjectiveId))
				{
					bProgressAwarded = QuestComponent->NotifyObjectiveProgress(
						QuestObjectiveId);
				}
			}
		};

		TryAwardProgress(LastQuestDamagePlayerState);

		if (!bProgressAwarded)
		{
			if (APawn* KillerPawn = GetInstigator())
			{
				TryAwardProgress(KillerPawn->GetPlayerState<AOPlayerState>());
			}
		}

		// Single-player/listen-server fallback. Only a player with a matching
		// active objective can receive progress, so unrelated quests are untouched.
		if (!bProgressAwarded)
		{
			if (UWorld* World = GetWorld())
			{
				for (FConstPlayerControllerIterator It =
					World->GetPlayerControllerIterator(); It && !bProgressAwarded; ++It)
				{
					if (APlayerController* PlayerController = It->Get())
					{
						TryAwardProgress(
							PlayerController->GetPlayerState<AOPlayerState>());
					}
				}
			}
		}

		if (!bProgressAwarded)
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[Quest] Boss %s died, but no player has active objective %s."),
				*GetName(), *QuestObjectiveId.ToString());
		}
	}

	GetWorldTimerManager().ClearTimer(PhaseTransitionTimer);
	SetBossInvulnerable(false);
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}
	SetBossPhase(EBossPhase::Dead);
	Super::Die();
}

void ABossCharacter::RecordQuestDamageSource(
	AController* SourceController, AActor* SourceActor)
{
	if (!HasAuthority() || QuestObjectiveId.IsNone()) return;

	AOPlayerState* SourcePlayerState = SourceController
		? SourceController->GetPlayerState<AOPlayerState>()
		: nullptr;
	if (!SourcePlayerState)
	{
		if (const APawn* SourcePawn = Cast<APawn>(SourceActor))
		{
			SourcePlayerState = SourcePawn->GetPlayerState<AOPlayerState>();
		}
	}

	if (SourcePlayerState)
	{
		LastQuestDamagePlayerState = SourcePlayerState;
	}
}

void ABossCharacter::SetBossPhase(EBossPhase NewPhase)
{
	if (BossPhase == NewPhase) return;

	const EBossPhase PreviousPhase = BossPhase;
	BossPhase = NewPhase;
	HandleBossPhaseChanged(PreviousPhase);
	ForceNetUpdate();
}

void ABossCharacter::OnRep_BossPhase(EBossPhase PreviousPhase)
{
	HandleBossPhaseChanged(PreviousPhase);
}

void ABossCharacter::HandleBossPhaseChanged(EBossPhase PreviousPhase)
{
	OnBossPhaseChanged.Broadcast(BossPhase);

	const bool bEncounterActive =
		BossPhase != EBossPhase::Dormant && BossPhase != EBossPhase::Dead;
	SetArenaBarriersEnabled(bEncounterActive);

	if (BossPhase == EBossPhase::Transition &&
		PreviousPhase != EBossPhase::Transition)
	{
		PlayPhaseTransition();
	}

	if (!bEncounterActive)
	{
		if (UWorld* World = GetWorld())
		{
			for (FConstPlayerControllerIterator It =
				World->GetPlayerControllerIterator(); It; ++It)
			{
				if (AOnePlayerController* PlayerController =
					Cast<AOnePlayerController>(It->Get()))
				{
					PlayerController->LeaveBossEncounter(this);
				}
			}
		}
	}
}

void ABossCharacter::PlayPhaseTransition()
{
	if (GetNetMode() == NM_DedicatedServer || !PhaseTransitionMontage) return;
	PlayAnimMontage(PhaseTransitionMontage);
}

void ABossCharacter::GrantBossAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& Abilities)
{
	if (!HasAuthority() || !AbilitySystemComponent) return;

	for (TSubclassOf<UGameplayAbility> AbilityClass : Abilities)
	{
		if (!AbilityClass ||
			AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass))
		{
			continue;
		}

		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(AbilityClass, GetPlayerLevel()));
	}
}
void ABossCharacter::SetArenaBarriersEnabled(bool bEnabled)
{
	for (AActor* Barrier : ArenaBarriers)
	{
		if (IsValid(Barrier))
		{
			Barrier->SetActorEnableCollision(bEnabled);
		}
	}
}

bool ABossCharacter::TryActivateBossAbility()
{
	if (!HasAuthority() || !AbilitySystemComponent || bIsDead) return false;
	if (BossPhase != EBossPhase::PhaseOne &&
		BossPhase != EBossPhase::PhaseTwo) return false;

	const TArray<TSubclassOf<UGameplayAbility>>& Pool =
		BossPhase == EBossPhase::PhaseTwo
		? PhaseTwoAbilities
		: PhaseOneAbilities;

	if (!Pool.IsEmpty())
	{
		const int32 StartIndex = FMath::RandRange(0, Pool.Num() - 1);
		for (int32 Offset = 0; Offset < Pool.Num(); ++Offset)
		{
			const int32 Index = (StartIndex + Offset) % Pool.Num();
			const TSubclassOf<UGameplayAbility> AbilityClass = Pool[Index];
			if (Pool.Num() > 1 && AbilityClass == LastActivatedBossAbility)
			{
				continue;
			}

			if (FGameplayAbilitySpec* Spec =
				AbilitySystemComponent->FindAbilitySpecFromClass(AbilityClass))
			{
				if (AbilitySystemComponent->TryActivateAbility(Spec->Handle))
				{
					LastActivatedBossAbility = AbilityClass;
					return true;
				}
			}
		}
	}

	FGameplayTagContainer AttackTags;
	AttackTags.AddTag(FPlayerGameplayTags::Get().Abilities_Attack);
	return AbilitySystemComponent->TryActivateAbilitiesByTag(AttackTags);
}

void ABossCharacter::ApplyDormantState()
{
	if (BossPhase != EBossPhase::Dormant) return;
	SetBossInvulnerable(true);
	GetCharacterMovement()->DisableMovement();
	if (PlayerAIController && PlayerAIController->GetBrainComponent())
	{
		PlayerAIController->GetBrainComponent()->PauseLogic(TEXT("Boss dormant"));
	}
}

void ABossCharacter::SetBossInvulnerable(bool bEnabled)
{
	if (!HasAuthority() || !AbilitySystemComponent ||
		bOwnsInvulnerabilityTag == bEnabled)
	{
		return;
	}

	const FGameplayTag InvulnerableTag =
		FPlayerGameplayTags::Get().State_Invulnerable;
	if (bEnabled)
	{
		AbilitySystemComponent->AddLooseGameplayTag(InvulnerableTag);
	}
	else
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(InvulnerableTag);
	}
	bOwnsInvulnerabilityTag = bEnabled;
}

void ABossCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(PhaseTransitionTimer);
	OnHealthChanged.RemoveDynamic(this, &ABossCharacter::HandleHealthChanged);
	BossEncounterTrigger->OnComponentBeginOverlap.RemoveDynamic(
		this, &ABossCharacter::HandleEncounterBeginOverlap);
	BossEncounterTrigger->OnComponentEndOverlap.RemoveDynamic(
		this, &ABossCharacter::HandleEncounterEndOverlap);
	SetArenaBarriersEnabled(false);

	if (UWorld* World = GetWorld())
	{
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			if (AOnePlayerController* PlayerController = Cast<AOnePlayerController>(It->Get()))
			{
				PlayerController->LeaveBossEncounter(this);
			}
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ABossCharacter::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABossCharacter, BossPhase);
}
