#include "Character/BossCharacter.h"

#include "AbilitySystem/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "AI/PlayerAIController.h"
#include "BrainComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerGameplayTags.h"
#include "TimerManager.h"
#include "Player/OnePlayerController.h"

ABossCharacter::ABossCharacter()
{
	BaseWalkSpeed = 0.f;
}

void ABossCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ApplyDormantState();
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();
	OnHealthChanged.AddDynamic(this, &ABossCharacter::HandleHealthChanged);
	ApplyDormantState();
}

void ABossCharacter::ActivateBoss()
{
	if (BossPhase != EBossPhase::Dormant || bIsDead) return;
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(FPlayerGameplayTags::Get().State_Invulnerable);
	}

	BaseWalkSpeed = PhaseOneWalkSpeed;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	if (PlayerAIController && PlayerAIController->GetBrainComponent())
	{
		PlayerAIController->GetBrainComponent()->ResumeLogic(TEXT("Boss activated"));
		PlayerAIController->GetBrainComponent()->RestartLogic();
	}
	SetBossPhase(EBossPhase::PhaseOne);
	if (AOnePlayerController* PC =
	Cast<AOnePlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->ShowBossHealthBar(this);
	}
}

void ABossCharacter::HandleHealthChanged(float NewHealth)
{
	if (BossPhase != EBossPhase::PhaseOne || !AttributeSet || NewHealth <= 0.f) return;

	const UPlayerAttributeSet* BossAttributes = Cast<UPlayerAttributeSet>(AttributeSet);
	if (BossAttributes && BossAttributes->GetMaxHealth() > 0.f &&
		NewHealth / BossAttributes->GetMaxHealth() <= PhaseTwoHealthPercent)
	{
		SetBossPhase(EBossPhase::Transition);
		GetCharacterMovement()->DisableMovement();
		if (PlayerAIController && PlayerAIController->GetBrainComponent())
		{
			PlayerAIController->GetBrainComponent()->PauseLogic(TEXT("Boss phase transition"));
		}
		PlayPhaseTransition();
		GetWorldTimerManager().SetTimer(
			PhaseTransitionTimer, this, &ABossCharacter::CompletePhaseTransition,
			PhaseTransitionDuration, false);
	}
}

void ABossCharacter::CompletePhaseTransition()
{
	if (BossPhase != EBossPhase::Transition || bIsDead) return;
	GetWorldTimerManager().ClearTimer(PhaseTransitionTimer);

	if (!bPhaseTwoAbilitiesGranted)
	{
		if (UPlayerAbilitySystemComponent* PlayerASC = Cast<UPlayerAbilitySystemComponent>(AbilitySystemComponent))
		{
			PlayerASC->AddCharacterAbilities(PhaseTwoAbilities);
		}
		bPhaseTwoAbilitiesGranted = true;
	}

	BaseWalkSpeed = PhaseTwoWalkSpeed;
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	if (PlayerAIController && PlayerAIController->GetBrainComponent())
	{
		PlayerAIController->GetBrainComponent()->ResumeLogic(TEXT("Boss phase two"));
	}
	SetBossPhase(EBossPhase::PhaseTwo);
}

void ABossCharacter::Die()
{
	if (bIsDead) return;
	SetBossPhase(EBossPhase::Dead);
	if (AOnePlayerController* PC =
	Cast<AOnePlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PC->HideBossHealthBar();
	}
	Super::Die();
}

void ABossCharacter::SetBossPhase(EBossPhase NewPhase)
{
	BossPhase = NewPhase;
	OnBossPhaseChanged.Broadcast(BossPhase);
}

void ABossCharacter::ApplyDormantState()
{
	if (BossPhase != EBossPhase::Dormant) return;
	if (AbilitySystemComponent && !AbilitySystemComponent->HasMatchingGameplayTag(FPlayerGameplayTags::Get().State_Invulnerable))
	{
		AbilitySystemComponent->AddLooseGameplayTag(FPlayerGameplayTags::Get().State_Invulnerable);
	}
	GetCharacterMovement()->DisableMovement();
	if (PlayerAIController && PlayerAIController->GetBrainComponent())
	{
		PlayerAIController->GetBrainComponent()->PauseLogic(TEXT("Boss dormant"));
	}
}
