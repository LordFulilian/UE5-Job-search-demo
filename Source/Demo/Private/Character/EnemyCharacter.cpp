// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/EnemyCharacter.h"
#include "Player/OPlayerState.h"      
#include "Components/ExpComponent.h"  
#include "Components/ItemPickup.h"   
#include "AbilitySystem/PlayerAttributeSet.h"
#include "AbilitySystem/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/PlayerAbilitySystemLibrary.h"
#include "PlayerGameplayTags.h"
#include "AI/PlayerAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"

#include "GameFramework/CharacterMovementComponent.h"


AEnemyCharacter::AEnemyCharacter()
{
	// Create the replicated ability-system component.
	AbilitySystemComponent = CreateDefaultSubobject<UPlayerAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;

	// Create the enemy attribute set.
	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));
	
	HealthBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetRootComponent());
}

void AEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (!HasAuthority()) return;
	PlayerAIController = Cast<APlayerAIController>(NewController);
	if (!PlayerAIController || !BehaviorTree || !BehaviorTree->BlackboardAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("%s requires PlayerAIController and a BehaviorTree with a Blackboard."), *GetName());
		return;
	}

	PlayerAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	PlayerAIController->RunBehaviorTree(BehaviorTree);
	PlayerAIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"),false);
	PlayerAIController->GetBlackboardComponent()->SetValueAsBool(FName("RangedAttacker"),CharacterClass == ECharacterClass::remote);
}

// 
void AEnemyCharacter::ToggleHighlight_Implementation(bool bActive)
{
	// Highlight every mesh component owned by the enemy.
	TArray<UMeshComponent*> MeshComponents;
	GetComponents<UMeshComponent>(MeshComponents);
    
	// Apply custom-depth settings to each mesh.
	for (UMeshComponent* Component : MeshComponents)
	{
		if (Component)
		{
			// Toggle custom-depth rendering.
			Component->SetRenderCustomDepth(bActive);

			// Stencil value 250 selects the enemy outline color.
			if (bActive)
			{
				Component->SetCustomDepthStencilValue(250);
			}
		}
	}
}

void AEnemyCharacter::Die()
{
	// Prevent re-entry (death animation plays, AI might trigger Die again)
	if (bIsDead) return;
	bIsDead = true;

	// Immediately kill AI to stop attacking during death animation
	if (PlayerAIController)
	{
		PlayerAIController->StopMovement();
		if (PlayerAIController->GetBrainComponent())
		{
			PlayerAIController->GetBrainComponent()->StopLogic(TEXT("Died"));
		}
	}

	SetLifeSpan(LifeSpan);

	if (APawn* KillerPawn = Cast<APawn>(GetInstigator()))
	{
		if (AOPlayerState* PS = KillerPawn->GetPlayerState<AOPlayerState>())
		{
			if (PS->ExpComponent)
			{
				PS->ExpComponent->AddExperience(50 * Level);
			}
		}
	}

	// Spawn loot drop at ground level (use mesh bone location, not capsule center)
	if (ItemPickupClass && HasAuthority())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		// Spawn at ground level: capsule center minus half height, plus small offset
		float CapsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		FVector SpawnLoc = GetActorLocation();
		SpawnLoc.Z -= CapsuleHalfHeight;
		SpawnLoc.Z += 15.f;
		GetWorld()->SpawnActor<AItemPickup>(ItemPickupClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
	}

	Super::Die();
}


void AEnemyCharacter::BeginPlay()

{

	Super::BeginPlay();
	HomeLocation = GetActorLocation();
	
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	InitAbilityActorInfo();

	UPlayerAbilitySystemLibrary::GiveStartupAbilities(this,AbilitySystemComponent,CharacterClass);

	HealthBar->InitWidget();



	if (UPlayerUserWidget * PlayerUserWidget= Cast<UPlayerUserWidget>(HealthBar->GetUserWidgetObject()))

	{

		PlayerUserWidget->SetWidgetController(this);

	}

	if (const UPlayerAttributeSet *PlayerAS= Cast<UPlayerAttributeSet>(AttributeSet))

	{

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAS->GetHealthAttribute()).AddLambda(

		[this](const FOnAttributeChangeData& Data)

		{

		OnHealthChanged.Broadcast(Data.NewValue);

		}

		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(PlayerAS->GetMaxHealthAttribute()).AddLambda(

		[this](const FOnAttributeChangeData& Data)

		{

		OnMaxHealthChanged.Broadcast(Data.NewValue);

		}

		);
		AbilitySystemComponent->RegisterGameplayTagEvent(FPlayerGameplayTags::Get().Effects_Hit_react,EGameplayTagEventType::NewOrRemoved).AddUObject(
		this,
		&AEnemyCharacter::HitReactTagChange
		);

		OnHealthChanged.Broadcast(PlayerAS->GetHealth());

		OnMaxHealthChanged.Broadcast(PlayerAS->GetMaxHealth());

	}

}

FVector AEnemyCharacter::SelectNextPatrolLocation()
{
	if (PatrolPoints.IsEmpty())
	{
		return HomeLocation;
	}

	for (int32 Attempt = 0; Attempt < PatrolPoints.Num(); ++Attempt)
	{
		PatrolPointIndex =
			(PatrolPointIndex + 1) % PatrolPoints.Num();

		AActor* PatrolPoint = PatrolPoints[PatrolPointIndex];

		if (IsValid(PatrolPoint))
		{
			return PatrolPoint->GetActorLocation();
		}
	}

	return HomeLocation;
}

void AEnemyCharacter::HitReactTagChange(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f: BaseWalkSpeed;
	if (PlayerAIController && PlayerAIController->GetBlackboardComponent())
	{
		PlayerAIController->GetBlackboardComponent()->SetValueAsBool(
			FName("HitReacting"), bHitReacting);
	}
	
}

void AEnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UPlayerAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	
	InitialzeDefaultAttributes();
}

int32 AEnemyCharacter::GetPlayerLevel()
{
	return Level;
}

void AEnemyCharacter::InitialzeDefaultAttributes() 
{
	UPlayerAbilitySystemLibrary::InitialzeDefaultAttributes(this,CharacterClass,Level,AbilitySystemComponent);
}
