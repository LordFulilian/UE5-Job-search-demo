#pragma once

#include "CoreMinimal.h"
#include "Character/EnemyCharacter.h"
#include "BossCharacter.generated.h"

class UGameplayAbility;
class UAnimMontage;
class UPrimitiveComponent;
class USphereComponent;
class AOPlayerState;

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Dormant,
	PhaseOne,
	Transition,
	PhaseTwo,
	Dead
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBossPhaseChangedSignature, EBossPhase, NewPhase);

UCLASS()
class DEMO_API ABossCharacter : public AEnemyCharacter
{
	GENERATED_BODY()

public:
	ABossCharacter();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Boss")
	void ActivateBoss();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Boss")
	void CompletePhaseTransition();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Boss|Combat")
	bool TryActivateBossAbility();

	UFUNCTION(BlueprintPure, Category = "Boss|Combat")
	float GetBossAttackRange() const { return BossAttackRange; }

	// Called by GAS damage execution while the source context is still available.
	void RecordQuestDamageSource(AController* SourceController, AActor* SourceActor);

	virtual void Die() override;
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(BlueprintAssignable, Category = "Boss")
	FBossPhaseChangedSignature OnBossPhaseChanged;

	UPROPERTY(ReplicatedUsing = OnRep_BossPhase, BlueprintReadOnly, Category = "Boss")
	EBossPhase BossPhase = EBossPhase::Dormant;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION()
	void HandleHealthChanged(float NewHealth);

	UFUNCTION()
	void OnRep_BossPhase(EBossPhase PreviousPhase);

	UFUNCTION()
	void HandleEncounterBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void HandleEncounterEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss|Encounter")
	TObjectPtr<USphereComponent> BossEncounterTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> PhaseOneAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Abilities")
	TArray<TSubclassOf<UGameplayAbility>> PhaseTwoAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Combat",
		meta = (ClampMin = "50.0", Units = "cm"))
	float BossAttackRange = 190.f;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Boss|Arena")
	TArray<TObjectPtr<AActor>> ArenaBarriers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss", meta = (ClampMin = "0.05", ClampMax = "0.95"))
	float PhaseTwoHealthPercent = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	float PhaseOneWalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	float PhaseTwoWalkSpeed = 450.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss", meta = (ClampMin = "0.0"))
	float PhaseTransitionDuration = 1.5f;

	// Stable ID used by the quest system when this Boss dies.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Quest")
	FName QuestObjectiveId = FName(TEXT("Boss.Aurora"));

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Boss|Animation")
	TObjectPtr<UAnimMontage> PhaseTransitionMontage;

private:
	void SetBossPhase(EBossPhase NewPhase);
	void HandleBossPhaseChanged(EBossPhase PreviousPhase);
	void PlayPhaseTransition();
	void ApplyDormantState();
	void SetBossInvulnerable(bool bEnabled);
	bool bPhaseTwoAbilitiesGranted = false;
	bool bOwnsInvulnerabilityTag = false;
	FTimerHandle PhaseTransitionTimer;

	UPROPERTY(Transient)
	TSubclassOf<UGameplayAbility> LastActivatedBossAbility;

	UPROPERTY(Transient)
	TObjectPtr<AOPlayerState> LastQuestDamagePlayerState;

	void GrantBossAbilities(
		const TArray<TSubclassOf<UGameplayAbility>>& Abilities);

	void SetArenaBarriersEnabled(bool bEnabled);
};
