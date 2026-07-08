

#include "Character/CharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/PlayerAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"



ACharacterBase::ACharacterBase()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	//创建武器骨骼网络
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	//将武器附加到角色的手部插槽上
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
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,true));

	if (DeathMontage && GetMesh() && GetMesh()->GetAnimInstance())
	{
		// Bind ragdoll to montage end — no timer gap, no "stand up" frame
		FOnMontageEnded EndDelegate;
		EndDelegate.BindWeakLambda(this, [this](UAnimMontage* Montage, bool bInterrupted)
		{
			if (!bInterrupted)
			{
				MulticasHamdleDeath();
			}
		});
		GetMesh()->GetAnimInstance()->Montage_Play(DeathMontage);
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate, DeathMontage);
	}
	else
	{
		MulticasHamdleDeath();
	}
}

void ACharacterBase::MulticasHamdleDeath_Implementation()
{
	// Disable capsule so it doesn't push the ragdoll
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Disable movement entirely
	GetCharacterMovement()->DisableMovement();

	// Weapon: drop with physics
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// Mesh: ragdoll with damped physics
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// Reduce explosion: apply linear damping
	GetMesh()->SetLinearDamping(0.5f);
	GetMesh()->SetAngularDamping(1.0f);
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






