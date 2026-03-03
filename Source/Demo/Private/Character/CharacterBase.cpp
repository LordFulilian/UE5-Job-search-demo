

#include "Character/CharacterBase.h"

#include "AbilitySystemComponent.h"


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

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACharacterBase::InitAbilityActorInfo()
{
}

void ACharacterBase::InitialzePrimaryAttributes() const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(DefaultPrimaryAttributes);
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
    const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultPrimaryAttributes,1.f,ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data,GetAbilitySystemComponent());
}



