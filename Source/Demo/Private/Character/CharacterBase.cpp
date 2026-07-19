

#include "Character/CharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/PlayerAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"



ACharacterBase::ACharacterBase()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	// Create and attach the weapon mesh.
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), "WeaponHandSocket");
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{

		return AbilitySystemComponent;

}

int32 ACharacterBase::GetPlayerLevel()
{
	return 1;
}

UAnimMontage* ACharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactAnimMontage;
}

void ACharacterBase::Die()
{
	if (bDeathSequenceStarted) return;
	bDeathSequenceStarted = true;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
	}
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->DisableMovement();
	}
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (Weapon)
	{
		Weapon->DetachFromComponent(
			FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	}

	float DeathPhysicsDelay = 0.25f;

	if (DeathMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		FOnMontageEnded EndDelegate;
		EndDelegate.BindWeakLambda(this, [this](UAnimMontage*, bool)
		{
			FinishDeathSequence();
		});

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		const float MontageDuration = AnimInstance->Montage_Play(DeathMontage);
		if (MontageDuration > 0.f)
		{
			AnimInstance->Montage_SetEndDelegate(EndDelegate, DeathMontage);
			DeathPhysicsDelay = MontageDuration + 0.25f;
		}
		else
		{
			FinishDeathSequence();
		}
	}
	else
	{
		FinishDeathSequence();
	}

	if (!bDeathPhysicsApplied)
	{
		GetWorldTimerManager().SetTimer(
			DeathPhysicsFallbackTimer,
			this,
			&ACharacterBase::FinishDeathSequence,
			DeathPhysicsDelay,
			false);
	}
}

void ACharacterBase::FinishDeathSequence()
{
	if (bDeathPhysicsApplied) return;

	if (HasAuthority())
	{
		MulticasHamdleDeath();
	}
	else
	{
		ApplyDeathPhysics();
	}
}

void ACharacterBase::MulticasHamdleDeath_Implementation()
{
	ApplyDeathPhysics();
}

void ACharacterBase::ApplyDeathPhysics()
{
	if (bDeathPhysicsApplied) return;
	bDeathPhysicsApplied = true;
	GetWorldTimerManager().ClearTimer(DeathPhysicsFallbackTimer);

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->DisableMovement();
	}

	if (Weapon)
	{
		Weapon->SetSimulatePhysics(true);
		Weapon->SetEnableGravity(true);
		Weapon->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	if (USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		// Stop the animation state machine from restoring an idle pose after
		// the death montage, while physics takes ownership of the bones.
		CharacterMesh->bPauseAnims = true;
		CharacterMesh->SetCollisionProfileName(TEXT("Ragdoll"));
		CharacterMesh->SetAllBodiesSimulatePhysics(true);
		CharacterMesh->SetEnableGravity(true);
		CharacterMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CharacterMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		CharacterMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		CharacterMesh->SetLinearDamping(0.5f);
		CharacterMesh->SetAngularDamping(1.0f);
	}
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterBase::InitAbilityActorInfo()
{
}

void ACharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) 
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass,Level,ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data,GetAbilitySystemComponent());
}

void ACharacterBase::InitialzeDefaultAttributes() 
{
	float CharacterLevel = static_cast<float>(GetPlayerLevel());
	ApplyEffectToSelf(DefaultPrimaryAttributes,CharacterLevel);
	ApplyEffectToSelf(DefaultSecondaryAttributes,CharacterLevel);
}



void ACharacterBase::AddCharacterAbilities()
{
	UPlayerAbilitySystemComponent* PlayerASC = CastChecked<UPlayerAbilitySystemComponent>(AbilitySystemComponent);
	if (!HasAuthority()) return;
	
	PlayerASC->AddCharacterAbilities(StartupAbilities);
}


