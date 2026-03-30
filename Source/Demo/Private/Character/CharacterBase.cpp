

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

void ACharacterBase::InitializeDefaultAttributes() 
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






