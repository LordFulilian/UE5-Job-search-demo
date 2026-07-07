#include "AbilitySystem/PlayerAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h" 
#include "PlayerGameplayTags.h"
#include "AbilitySystem/PlayerAbilitySystemComponent.h"
#include "AbilitySystem/PlayerAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "interaction/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/OnePlayerController.h"

UPlayerAttributeSet::UPlayerAttributeSet()
{
    // 获取全局 Tag 单例
    const FPlayerGameplayTags& GameplayTags = FPlayerGameplayTags::Get();

    // 绑定核心生存属性
    TagsToAttributes.Add(GameplayTags.Attributes_Vital_Health, GetHealthAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Vital_MaxHealth, GetMaxHealthAttribute);

    // 绑定一级面板属性
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Attack, GetAttackAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Primary_Defense, GetDefenseAttribute);

    // 绑定二级进阶属性
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CritRate, GetCritRateAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CritDamage, GetCritDamageAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Secondary_EnergyRegen, GetEnergyRegenAttribute);

    // 绑定三级增伤属性
    TagsToAttributes.Add(GameplayTags.Attributes_DamageBonus_SkillDamageBonus, GetSkillDamageBonusAttribute);

    // 绑定抗性属性
    TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Ice, GetIceResistanceAttribute);
    TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
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
    
    // 抗性 
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, FireResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, IceResistance, COND_None, REPNOTIFY_Always);
    DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, PhysicalResistance, COND_None, REPNOTIFY_Always);
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
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Health, OldHealth);
}

void UPlayerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxHealth, OldMaxHealth);
}

void UPlayerAttributeSet::OnRep_Attack(const FGameplayAttributeData& OldAttack) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Attack, OldAttack);
}

void UPlayerAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Defense, OldDefense);
}

void UPlayerAttributeSet::OnRep_CritRate(const FGameplayAttributeData& OldCritRate) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, CritRate, OldCritRate);
}

void UPlayerAttributeSet::OnRep_CritDamage(const FGameplayAttributeData& OldCritDamage) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, CritDamage, OldCritDamage);
}

void UPlayerAttributeSet::OnRep_EnergyRegen(const FGameplayAttributeData& OldEnergyRegen) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, EnergyRegen, OldEnergyRegen);
}

void UPlayerAttributeSet::OnRep_SkillDamageBonus(const FGameplayAttributeData& OldSkillDamageBonus) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, SkillDamageBonus, OldSkillDamageBonus);
}

void UPlayerAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, FireResistance, OldFireResistance);
}

void UPlayerAttributeSet::OnRep_IceResistance(const FGameplayAttributeData& OldIceResistance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, IceResistance, OldIceResistance);
}

void UPlayerAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

void UPlayerAttributeSet::SetEffectProperties(const struct FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
    Props.EffectContextHandle = Data.EffectSpec.GetContext();
    Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
    
    if (IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid())
    {
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

   if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
   {
      // 1. 把暂存篮子里的伤害值拿出来
      const float LocalIncomingDamage = GetIncomingDamage();
        
      // 2. 立刻清空篮子
      SetIncomingDamage(0.f);

      // 3. 执行扣血逻辑
      if (LocalIncomingDamage > 0.f)
      {
         const float NewHealth = GetHealth() - LocalIncomingDamage;
         SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
      	
      	if (GEngine)
      	{
      		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("【底层扣血成功】当前真实剩余血量: %f"), GetHealth()));
      	}
         
         const bool bFatal = NewHealth <= 0.f;
         
         if (bFatal)
         {
            ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
            if (CombatInterface)
            {
               CombatInterface->Die();
            }
         }
         else
         {
            FGameplayTagContainer TagContainer;
            TagContainer.AddTag(FPlayerGameplayTags::Get().Effects_Hit_react);
            Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
         }
         
         const bool bCriticalHit = UPlayerAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
         ShowFloatingText(Props,LocalIncomingDamage,bCriticalHit);
      }
   }
}

void UPlayerAttributeSet::ShowFloatingText(const FEffectProperties& Props, float Damage, bool bCriticalHit) const
{
	if (Props.SourceCharacter != Props.TargetCharacter)
	{
		// 1. 尝试拿攻击方的控制器（玩家打怪时生效）
		AOnePlayerController* PC = Cast<AOnePlayerController>(Props.SourceController);
        
		// 2. 如果拿不到（说明是怪物打玩家），那就拿挨打方的控制器
		if (!PC)
		{
			PC = Cast<AOnePlayerController>(Props.TargetController);
		}
        
		// 3. 弹伤害数字
		if (PC)
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter, bCriticalHit);
		}
	}
}