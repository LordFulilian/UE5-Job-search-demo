// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OPlayerState.h"
#include <AbilitySystem/PlayerAbilitySystemComponent.h>
#include <AbilitySystem/PlayerAttributeSet.h>
#include "Components/ExpComponent.h"
#include "Components/PartyComponent.h"
#include "Components/QuestComponent.h"
#include "Net/UnrealNetwork.h"

AOPlayerState::AOPlayerState()
{
	
	AbilitySystemComponent = CreateDefaultSubobject<UPlayerAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	
	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));

	ExpComponent = CreateDefaultSubobject<UExpComponent>(TEXT("ExpComponent"));
	QuestComponent = CreateDefaultSubobject<UQuestComponent>(TEXT("QuestComponent"));
	PartyComponent = CreateDefaultSubobject<UPartyComponent>(TEXT("PartyComponent"));
	

	NetUpdateFrequency = 100.f;
}

void AOPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AOPlayerState, Level);
}

void AOPlayerState::OnRep_Level(int32 OldLevel)
{
}

UAbilitySystemComponent* AOPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
