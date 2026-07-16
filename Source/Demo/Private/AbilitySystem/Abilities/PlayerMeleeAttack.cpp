#include "AbilitySystem/Abilities/PlayerMeleeAttack.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "DrawDebugHelpers.h" 
#include "PlayerGameplayTags.h"

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

    // Validate combo configuration before creating ability tasks.
    if (MaxComboCount > 1 && !ComboEventTag.IsValid())
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("⚠️ 提示: 设定为多段攻击，但未配置 ComboEventTag"));
    }

    ComboCount = 1;
    bSaveAttack = false;
    // Reinitialize combo state for this activation.
    if (MaxComboCount > 1 && !ComboEventTag.IsValid())
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, TEXT("⚠️ 提示: 设定为多段攻击，但未配置 ComboEventTag"));
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
    else
    {
        if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, TEXT("⏳ 没按键，连招结束，自然收招。"));
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

    FHitResult HitResult;
    bool bHit = World->SweepSingleByChannel(HitResult, StartLocation, EndLocation, FQuat::Identity, ECollisionChannel::ECC_Pawn, SphereShape, QueryParams);

#if WITH_EDITOR
    DrawDebugSphere(World, StartLocation, TraceRadius, 12, FColor::Green, false, 2.f);
    if (bHit) DrawDebugSphere(World, HitResult.ImpactPoint, TraceRadius, 12, FColor::Red, false, 2.f);
#endif

    if (bHit && HitResult.GetActor())
    {
       AActor* TargetActor = HitResult.GetActor();
       UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
       UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

       if (!TargetASC || !DamageEffectClass) return;

       FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
       EffectContext.AddHitResult(HitResult);
       FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContext);

        if (SpecHandle.IsValid())
        {
           FPlayerGameplayTags GameplayTags = FPlayerGameplayTags::Get(); 
           const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
           UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage_Physical, ScaledDamage);
           SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
        }
    }
}
