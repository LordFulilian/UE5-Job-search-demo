#include "Actor/PlayerEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
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
    if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;

    UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
    if (TargetASC == nullptr) return;

    check(GameplayEffectClass)
    FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
    EffectContextHandle.AddSourceObject(this);
    const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
    const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

    const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
    if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnOverlap)
    {
        ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
    }

    if (!bIsInfinite)
    {
        Destroy();
    }
}

void APlayerEffectActor::OnOverlap(AActor* TargetActor)
{
    if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;

    // Apply instant effects configured for overlap entry.
    if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
        ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
    }

    // Apply duration effects configured for overlap entry.
    if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
        ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
    }

    // Apply infinite effects configured for overlap entry.
    if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
    {
        ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
    }
}

void APlayerEffectActor::OnEndOverlap(AActor* TargetActor)
{
    if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;

    // Apply instant effects configured for overlap exit.
    if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
        ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
    }

    // Apply duration effects configured for overlap exit.
    if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
        ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
    }

    // Apply infinite effects configured for overlap exit.
    if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
    {
        ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
    }

    // Remove infinite effects owned by the actor leaving the overlap.
    if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnOverlap)
    {
        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
        if (!IsValid(TargetASC)) return;

        TArray<FActiveGameplayEffectHandle> HandlesToRemove;
        for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
        {
            if (TargetASC == HandlePair.Value)
            {
                TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
                HandlesToRemove.Add(HandlePair.Key);
            }
        }

        // Remove map entries after iteration to avoid invalidating the iterator.
        for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
        {
            ActiveEffectHandles.Remove(Handle);
        }
    }
}
