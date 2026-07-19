#include "AbilitySystem/Abilities/PlayerMeleeAttack.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "PlayerGameplayTags.h"
#include "interaction/EnemyInterface.h"

namespace PlayerCollisionChannels
{
    // Keep these aligned with DefaultEngine.ini's Player and Enemy object channels.
    constexpr ECollisionChannel Player = ECC_GameTraceChannel1;
    constexpr ECollisionChannel Enemy = ECC_GameTraceChannel2;
}

UPlayerMeleeAttack::UPlayerMeleeAttack()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPlayerMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!AttackMontage)
    {
       EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
       return;
    }

    // Invalid combo data degrades to a single attack.
    if (MaxComboCount > 1 && !ComboEventTag.IsValid())
    {
        MaxComboCount = 1;
    }

    ComboCount = 1;
    bSaveAttack = false;

    // Listen for the montage hit event.
    if (HitEventTag.IsValid())
    {
        UAbilityTask_WaitGameplayEvent* WaitHitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, HitEventTag, nullptr, false, true);
        if (WaitHitEventTask)
        {
           WaitHitEventTask->EventReceived.AddDynamic(this, &UPlayerMeleeAttack::OnHitEventReceived);
           WaitHitEventTask->ReadyForActivation();
        }
    }

    // Listen for montage combo-window events.
    if (ComboEventTag.IsValid())
    {
        // Accept child tags as combo-window events.
        UAbilityTask_WaitGameplayEvent* WaitComboEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, ComboEventTag, nullptr, false, false);
        if (WaitComboEventTask)
        {
           WaitComboEventTask->EventReceived.AddDynamic(this, &UPlayerMeleeAttack::OnComboEventReceived);
           WaitComboEventTask->ReadyForActivation();
        }
    }

    // Play the attack montage and end the ability on every terminal outcome.
    UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);
    if (MontageTask)
    {
       MontageTask->OnBlendOut.AddDynamic(this, &UPlayerMeleeAttack::OnMontageCompleted);
       MontageTask->OnCompleted.AddDynamic(this, &UPlayerMeleeAttack::OnMontageCompleted);
       MontageTask->OnInterrupted.AddDynamic(this, &UPlayerMeleeAttack::OnMontageCompleted);
       MontageTask->OnCancelled.AddDynamic(this, &UPlayerMeleeAttack::OnMontageCompleted);
       
       MontageTask->ReadyForActivation();
    }

    // Start from the first combo section when it exists.
    if (UAnimInstance* AnimInstance = CurrentActorInfo->GetAnimInstance())
    {
        FName FirstSection("Attack1");
        if (AttackMontage->IsValidSectionName(FirstSection))
        {
            AnimInstance->Montage_JumpToSection(FirstSection, AttackMontage);
        }
    }

    // Fall back to a timed trace if no montage event arrives.
    if (HitTraceDelay > 0.f && GetWorld())
    {
        bTraceAlreadyFired = false;
        GetWorld()->GetTimerManager().SetTimer(HitTraceTimerHandle, this, &UPlayerMeleeAttack::OnHitTraceTimerFired, HitTraceDelay, false);
    }
}

// Buffer native input received while the ability is active.
void UPlayerMeleeAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    Super::InputPressed(Handle, ActorInfo, ActivationInfo);

    // Save one input for the next available combo window.
    if (ComboCount < MaxComboCount)
    {
        bSaveAttack = true;
       
    }
}

void UPlayerMeleeAttack::OnComboEventReceived(FGameplayEventData Payload)
{


    // Advance only when input was buffered before the combo event.
    if (bSaveAttack)
    {
        bSaveAttack = false; // Consume buffered input.
        ComboCount++;        // Advance to the next attack.

        // Allow the next combo section to deal damage.
        bTraceAlreadyFired = false; 

        // Montage sections follow the Attack2, Attack3 naming convention.
        FName NextSection = FName(*FString::Printf(TEXT("Attack%d"), ComboCount));
        
       
        if (UAnimInstance* AnimInstance = CurrentActorInfo->GetAnimInstance())
        {
            AnimInstance->Montage_JumpToSection(NextSection, AttackMontage);
        }
    }
}

void UPlayerMeleeAttack::OnMontageCompleted()
{
    if (GetWorld() && HitTraceTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(HitTraceTimerHandle);
    }
    bTraceAlreadyFired = false;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UPlayerMeleeAttack::OnHitTraceTimerFired()
{
    PerformMeleeTraceAndApplyDamage();
}

void UPlayerMeleeAttack::OnHitEventReceived(FGameplayEventData Payload)
{
    

    if (GetWorld() && HitTraceTimerHandle.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(HitTraceTimerHandle);
    }
    PerformMeleeTraceAndApplyDamage();
}

void UPlayerMeleeAttack::PerformMeleeTraceAndApplyDamage()
{
    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (!AvatarActor) return;

    if (bTraceAlreadyFired) return;
    bTraceAlreadyFired = true;

    UWorld* World = GetWorld();
    if (!World) return;

    FVector StartLocation = AvatarActor->GetActorLocation();
    FVector ForwardVector = AvatarActor->GetActorForwardVector();
    FVector EndLocation = StartLocation + (ForwardVector * TraceDistance);

    FCollisionShape SphereShape = FCollisionShape::MakeSphere(TraceRadius);
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(AvatarActor); 

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
    ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
    ObjectQueryParams.AddObjectTypesToQuery(PlayerCollisionChannels::Player);
    ObjectQueryParams.AddObjectTypesToQuery(PlayerCollisionChannels::Enemy);

    TArray<FHitResult> HitResults;
    const bool bHit = World->SweepMultiByObjectType(
        HitResults,
        StartLocation,
        EndLocation,
        FQuat::Identity,
        ObjectQueryParams,
        SphereShape,
        QueryParams);

    if (!bHit) return;

    const bool bSourceIsEnemy = AvatarActor->Implements<UEnemyInterface>();

    for (const FHitResult& HitResult : HitResults)
    {
       AActor* TargetActor = HitResult.GetActor();
       if (!IsValid(TargetActor))
       {
           continue;
       }

       UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
       UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

       const bool bTargetIsEnemy = TargetActor->Implements<UEnemyInterface>();
       if (bSourceIsEnemy == bTargetIsEnemy ||
           !SourceASC || !TargetASC || !DamageEffectClass)
       {
           continue;
       }

       FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
       EffectContext.AddHitResult(HitResult);
       FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContext);

        if (SpecHandle.IsValid())
        {
           FPlayerGameplayTags GameplayTags = FPlayerGameplayTags::Get(); 
           const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
           UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage_Physical, ScaledDamage);
           SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

           break;
        }
    }
}
