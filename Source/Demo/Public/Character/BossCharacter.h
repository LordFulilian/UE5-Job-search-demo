#pragma once

#include "CoreMinimal.h"
#include "Character/EnemyCharacter.h"
#include "BossCharacter.generated.h"

class UGameplayAbility;

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

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void ActivateBoss();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	void CompletePhaseTransition();

	virtual void Die() override;

	UPROPERTY(BlueprintAssignable, Category = "Boss")
	FBossPhaseChangedSignature OnBossPhaseChanged;

	UPROPERTY(BlueprintReadOnly, Category = "Boss")
	EBossPhase BossPhase = EBossPhase::Dormant;

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
	void PlayPhaseTransition();

	UFUNCTION()
	void HandleHealthChanged(float NewHealth);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss", meta = (ClampMin = "0.05", ClampMax = "0.95"))
	float PhaseTwoHealthPercent = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	TArray<TSubclassOf<UGameplayAbility>> PhaseTwoAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	float PhaseOneWalkSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	float PhaseTwoWalkSpeed = 450.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss", meta = (ClampMin = "0.0"))
	float PhaseTransitionDuration = 1.5f;

private:
	void SetBossPhase(EBossPhase NewPhase);
	void ApplyDormantState();
	bool bPhaseTwoAbilitiesGranted = false;
	FTimerHandle PhaseTransitionTimer;
};
