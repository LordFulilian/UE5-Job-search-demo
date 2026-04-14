#include "AbilitySystem/Abilities/PlayerMeleeAttack.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "DrawDebugHelpers.h" // 添加这个头文件以支持 DrawDebugSphere

UPlayerMeleeAttack::UPlayerMeleeAttack()
{
    // 实例化策略：播放动画的技能必须是 "InstancedPerActor" (每个角色实例化一个)
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPlayerMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    // 必须调用父类
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // 如果忘了在蓝图里配置动画，直接结束技能，防止卡死
    if (!AttackMontage)
    {
       EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
       return;
    }

    // --- 1. 启动监听动画事件的任务 ---
    // OnlyMatchExact: true 表示必须完全匹配我们设置的 Tag
    UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, HitEventTag, nullptr, false, true);
    if (WaitEventTask)
    {
       // 当监听到事件时，触发 OnHitEventReceived 函数
       WaitEventTask->EventReceived.AddDynamic(this, &UPlayerMeleeAttack::OnHitEventReceived);
       WaitEventTask->ReadyForActivation();
    }

    // --- 2. 启动播放蒙太奇的任务 ---
    UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);
    if (MontageTask)
    {
       // 使用 C++ 动态委托，监听动画的状态
       MontageTask->OnBlendOut.AddDynamic(this, &UPlayerMeleeAttack::OnMontageCompleted);
       MontageTask->OnCompleted.AddDynamic(this, &UPlayerMeleeAttack::OnMontageCompleted);
       MontageTask->OnInterrupted.AddDynamic(this, &UPlayerMeleeAttack::OnMontageCompleted);
       MontageTask->OnCancelled.AddDynamic(this, &UPlayerMeleeAttack::OnMontageCompleted);
       
       MontageTask->ReadyForActivation();
    }
}

void UPlayerMeleeAttack::OnMontageCompleted()
{
    // 动画播放完毕，调用底层函数正式结束当前技能。
    // 这样 GAS 系统才会知道你砍完了，允许你进行下一次攻击或释放别的技能。
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UPlayerMeleeAttack::OnHitEventReceived(FGameplayEventData Payload)
{
    // 动画播到了受击帧！立刻执行射线和伤害检测！
    PerformMeleeTraceAndApplyDamage();
}

void UPlayerMeleeAttack::PerformMeleeTraceAndApplyDamage()
{
    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (!AvatarActor) return;

    UWorld* World = GetWorld();
    if (!World) return;

    // 1. 计算射线起点与终点
    FVector StartLocation = AvatarActor->GetActorLocation();
    FVector ForwardVector = AvatarActor->GetActorForwardVector();
    FVector EndLocation = StartLocation + (ForwardVector * TraceDistance);

    // 2. 设置碰撞形状和查询参数
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(TraceRadius);
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(AvatarActor); // 忽略自身

    FHitResult HitResult;

    // 3. 执行物理射线检测 (对应蓝图的 Sphere Trace By Channel)
    bool bHit = World->SweepSingleByChannel(
       HitResult, 
       StartLocation, 
       EndLocation, 
       FQuat::Identity, 
       ECC_Visibility, 
       SphereShape, 
       QueryParams
    );

    // 绘制 Debug 线，方便在编辑器里调试 (打包发布时会自动剔除)
#if WITH_EDITOR
    DrawDebugSphere(World, StartLocation, TraceRadius, 12, FColor::Green, false, 2.f);
    if (bHit)
    {
       DrawDebugSphere(World, HitResult.ImpactPoint, TraceRadius, 12, FColor::Red, false, 2.f);
    }
#endif

    // 4. 处理命中逻辑
    // 4. 处理命中逻辑
    if (bHit && HitResult.GetActor())
    {
       AActor* TargetActor = HitResult.GetActor();
       UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
       UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

       // 诊断 1：检查有没有 ASC 组件
       if (!TargetASC)
       {
           if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("❌ 失败：你打中的东西身上没有 ASC 组件！"));
           return;
       }

       // 诊断 2：检查蓝图里有没有配置 GE
       if (!DamageEffectClass)
       {
           if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("❌ 失败：代码里 DamageEffectClass 为空！请去蓝图重新配置 GE_MeleeDamage！"));
           return;
       }

       // 记录砍之前的血量
       bool bFoundHealth = false;
       float OldHealth = TargetASC->GetGameplayAttributeValue(UPlayerAttributeSet::GetHealthAttribute(), bFoundHealth);

       // 构建并应用伤害
       FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
       EffectContext.AddHitResult(HitResult);
       FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContext);

       // 🔴 核心：捕获 GE 的应用结果
       FActiveGameplayEffectHandle ActiveHandle = SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

       // 诊断 3：分析应用结果
       if (ActiveHandle.WasSuccessfullyApplied())
       {
           float NewHealth = TargetASC->GetGameplayAttributeValue(UPlayerAttributeSet::GetHealthAttribute(), bFoundHealth);
           if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("✅ 成功！扣血前: %.1f, 扣血后: %.1f"), OldHealth, NewHealth));
       }
       else
       {
           if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("❌ 失败：GE 被引擎拦截，无法应用！"));
           
           // 诊断 4：是不是网络权限惹的祸？
           if (!AvatarActor->HasAuthority())
           {
               if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Yellow, TEXT("⚠️ 原因找到：你当前是【客户端(Client)】。GAS 规定客户端无权扣血，必须在服务器执行！"));
           }
       }
    }
    
}