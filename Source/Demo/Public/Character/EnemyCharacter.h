// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Components/WidgetComponent.h"
#include "interaction/EnemyInterface.h" 
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "EnemyCharacter.generated.h"

class AActor;
class UUserWidget;
class UBehaviorTree;
class APlayerAIController;
/**
 * Enemy character controlled by AI.
 */
UCLASS()

class DEMO_API AEnemyCharacter : public ACharacterBase, public IEnemyInterface 
{
	GENERATED_BODY()

public:
	AEnemyCharacter();
	virtual void PossessedBy( AController * NewController) override;
	
	virtual void ToggleHighlight_Implementation(bool bActive) override;
	
	virtual void Die() override;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	void HitReactTagChange(const FGameplayTag CallbackTag,int32 NewCount);
	
	UPROPERTY(BlueprintReadOnly,Category = "Combat")
	bool bHitReacting = false;
	
	bool bIsDead = false;
	
	UPROPERTY(BlueprintReadOnly,Category = "Combat")
	float BaseWalkSpeed = 300.0f;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	TSubclassOf<class AItemPickup> ItemPickupClass;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	float LifeSpan= 5.f;	
	
	UFUNCTION(BlueprintPure, Category = "AI|Navigation")
	FVector GetHomeLocation() const
	{
		return HomeLocation;
	}

	void SetHomeLocation(const FVector& NewHomeLocation)
	{
		HomeLocation = NewHomeLocation;
	}

	UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
	FVector SelectNextPatrolLocation();

	UFUNCTION(BlueprintPure, Category = "AI|Navigation")
	float GetAggroRadius() const
	{
		return AggroRadius;
	}

	UFUNCTION(BlueprintPure, Category = "AI|Navigation")
	float GetMaxChaseDistance() const
	{
		return MaxChaseDistance;
	}

	UFUNCTION(BlueprintPure, Category = "AI|Navigation")
	float GetHomeAcceptanceRadius() const
	{
		return HomeAcceptanceRadius;
	}

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual int32 GetPlayerLevel() override;
	virtual  void InitialzeDefaultAttributes()  override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	ECharacterClass CharacterClass = ECharacterClass::longsword;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
	
	UPROPERTY(EditAnywhere, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "AI|Patrol")
	TArray<TObjectPtr<AActor>> PatrolPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Patrol",
		meta = (ClampMin = "0.0"))
	float AggroRadius = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Patrol",
		meta = (ClampMin = "0.0"))
	float MaxChaseDistance = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Patrol",
		meta = (ClampMin = "0.0"))
	float HomeAcceptanceRadius = 100.f;

	UPROPERTY()
	TObjectPtr<APlayerAIController> PlayerAIController;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly,
		Category = "AI|Patrol",
		meta = (AllowPrivateAccess = "true"))
	FVector HomeLocation = FVector::ZeroVector;

	int32 PatrolPointIndex = INDEX_NONE;
};
