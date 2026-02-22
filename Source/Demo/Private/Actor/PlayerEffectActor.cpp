

#include "Actor/PlayerEffectActor.h"
#include  "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/PlayerAttributeSet.h"



APlayerEffectActor::APlayerEffectActor()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("RootSceneComponent"));
}


void APlayerEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr)return;
	
	check(GameplayEffectClass)
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass,1.f,EffectContextHandle);
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}




