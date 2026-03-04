#include "AbilitySystem/PlayerAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h" 
#include "GameFramework/Character.h"

UPlayerAttributeSet::UPlayerAttributeSet()
{
    // 初始化基础血量 (其他属性可以通过 GE_InitPlayerAttributes 蓝图来初始化)
    InitHealth(50.f);
    InitMaxHealth(100.f);
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    // 核心生存属性
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Health, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
    
    // 一级面板属性
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Attack, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Defense, COND_None, REPNOTIFY_Always);
    
    // 二级进阶属性
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, CritRate, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, CritDamage, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, EnergyRegen, COND_None, REPNOTIFY_Always);
    
    // 三级增伤属性
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, SkillDamageBonus, COND_None, REPNOTIFY_Always);
}

void UPlayerAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
    Super::PreAttributeBaseChange(Attribute, NewValue);
    
    // 在数值修改前拦截，防止血量溢出上限或变成负数
    if (Attribute == GetHealthAttribute())
    {
       NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth()); 
    }
}

void UPlayerAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
}

void UPlayerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
}

void UPlayerAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack) const
{
}

void UPlayerAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense) const
{
}

void UPlayerAttributeSet::OnRep_CritRate(const FGameplayAttributeData& OldCritRate) const
{
}

void UPlayerAttributeSet::OnRep_CritDamage(const FGameplayAttributeData& OldCritDamage) const
{
}

void UPlayerAttributeSet::OnRep_EnergyRegen(const FGameplayAttributeData& OldEnergyRegen) const
{
}

void UPlayerAttributeSet::OnRep_SkillDamageBonus(const FGameplayAttributeData& OldSkillDamageBonus) const
{
}

void UPlayerAttributeSet::SetEffectProperties(const struct FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
    Props.EffectContextHandle = Data.EffectSpec.GetContext();
    Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
    
    if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid())
    {
       // 【已修复】直接赋值给 Props 里的变量，而不是声明局部临时变量
       Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();
       Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();
       
       if (Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
       {
          if (const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
          {
             Props.SourceController = Pawn->GetController();
          }
       }
       if (Props.SourceController)
       {
          
          Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
       }
    }
    
    if (Data.Target.AbilityActorInfo.IsValid())
    {
       Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get(); 
       Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
       Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
       Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor); 
    }
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
    
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
}