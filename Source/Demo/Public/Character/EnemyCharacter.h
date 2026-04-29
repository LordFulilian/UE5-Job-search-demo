// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Components/WidgetComponent.h"
#include "interaction/EnemyInterface.h" 
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "EnemyCharacter.generated.h"


class UUserWidget;
/**
 * 敌人角色类
 */
UCLASS()

class DEMO_API AEnemyCharacter : public ACharacterBase, public IEnemyInterface 
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

	
	virtual void ToggleHighlight_Implementation(bool bActive) override;
	
	virtual void Die() override;
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSinature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSinature OnMaxHealthChanged;
	
	void HitReactTagChange(const FGameplayTag CallbackTag,int32 NewCount);
	
	UPROPERTY(BlueprintReadOnly,Category = "Combat")
	bool bHitReacting = false;
	
	UPROPERTY(BlueprintReadOnly,Category = "Combat")
	float BaseWalkSpeed = 300.0f;	
	
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Combat")
	float LifeSpan= 5.f;	
	
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
};