// Rylan


#include "AbilitySystem/Abilities/PlayerMeleeAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UPlayerMeleeAttack::UPlayerMeleeAttack()
{
	// 实例化策略：播放动画的技能必须是 "InstancedPerActor" (每个角色实例化一个)
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPlayerMeleeAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// 必须调用父类
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 1. 如果你忘了在蓝图里配置动画，直接结束技能，防止卡死
	if (!AttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 2. 🔴 创建一个播放蒙太奇的 GAS 异步任务
	UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AttackMontage);

	if (Task)
	{
		// 3. 🔴 使用 C++ 动态委托，监听动画的状态。
		// 无论是正常播完 (Completed)、被其他技能打断 (Interrupted)、还是被取消，我们都让它去执行 OnMontageCompleted 函数
		Task->OnBlendOut.AddDynamic(this, &UPlayerMeleeAttack::OnMontageCompleted);
		Task->OnCompleted.AddDynamic(this, &UPlayerMeleeAttack::OnMontageCompleted);
		Task->OnInterrupted.AddDynamic(this, &UPlayerMeleeAttack::OnMontageCompleted);
		Task->OnCancelled.AddDynamic(this, &UPlayerMeleeAttack::OnMontageCompleted);

		// 4. 正式启动这个任务，你的角色就会开始挥刀了！
		Task->ReadyForActivation();
	}
}

void UPlayerMeleeAttack::OnMontageCompleted()
{
	// 5. 动画播放完毕，调用底层函数正式结束当前技能。
	// 这样 GAS 系统才会知道你砍完了，允许你进行下一次攻击或释放别的技能。
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
