// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PlayerGameModeBase.generated.h"

class UCharacterClassInfo;
/**
 * 
 */
UCLASS()
class DEMO_API APlayerGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly,Category="Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;
};
