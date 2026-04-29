

#include "Character/CharacterBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/PlayerAbilitySystemComponent.h"



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
	MulticasHamdleDeath();
}

void ACharacterBase::MulticasHamdleDeath_Implementation()
{
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
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






