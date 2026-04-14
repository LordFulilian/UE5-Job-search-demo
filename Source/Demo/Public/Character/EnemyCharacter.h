// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Components/WidgetComponent.h"
#include "interaction/EnemyInterface.h" 
#include "UI/WidgetController/OverlayWidgetController.h"
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
	
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSinature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSinature OnMaxHealthChanged;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual int32 GetPlayerLevel() override;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;
};