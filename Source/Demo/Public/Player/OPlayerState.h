// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "OPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UPartyComponent;
class UQuestComponent;

UCLASS(BlueprintType, Blueprintable)
class DEMO_API AOPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AOPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFUNCTION(BlueprintPure, Category = "Ability")
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	UFUNCTION(BlueprintPure, Category = "Quest")
	UQuestComponent* GetQuestComponent() const { return QuestComponent; }

	UFUNCTION(BlueprintPure, Category = "Party")
	UPartyComponent* GetPartyComponent() const { return PartyComponent; }
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UExpComponent> ExpComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UQuestComponent> QuestComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPartyComponent> PartyComponent;
	
	UFUNCTION(BlueprintPure, Category = "Progression")
	int32 GetPlayerLevel() const { return Level; }
protected:
	//
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent>AbilitySystemComponent;

	//
	UPROPERTY()
	TObjectPtr<UAttributeSet>AttributeSet;
private:
	UPROPERTY(VisibleAnywhere,ReplicatedUsing=OnRep_Level)
	int32 Level = 1;
	
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
};
