// Rylan

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "PlayerGameplayTags.h"
#include "AbilitySystem/PlayerAttributeSet.h"

struct PlayerDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Attack);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CritRate);
    // 🔴 修复 1：补上暴击伤害的声明
    DECLARE_ATTRIBUTE_CAPTUREDEF(CritDamage); 
    
    PlayerDamageStatics()
    {
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Defense, Target, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Attack, Source, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, CritRate, Source, false);
       // 🔴 修复 1：补上暴击伤害的宏定义 (暴击伤害也是攻击者的，所以是 Source)
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, CritDamage, Source, false); 
    }
};

static const PlayerDamageStatics DamageStatics()
{
    static PlayerDamageStatics Static;
    return Static;
}

UExecCalc_Damage::UExecCalc_Damage()
{
    RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
    RelevantAttributesToCapture.Add(DamageStatics().AttackDef);
    RelevantAttributesToCapture.Add(DamageStatics().CritRateDef);
    RelevantAttributesToCapture.Add(DamageStatics().CritDamageDef); 
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
    
    const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
    
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    
    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;
    
    // 1. 获取攻击者传来的原始基础伤害 (SetByCaller)
    float BaseDamage = Spec.GetSetByCallerMagnitude(FPlayerGameplayTags::Get().Damage);
    
    // 2. 抓取防御力 (Target)
    float Defense = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluateParameters, Defense);
    Defense = FMath::Max<float>(0.f, Defense);
    
    // 3. 抓取攻击力 (Source)
    float Attack = 0.f;
    // 🔴 修复 2：最后一个参数改回 Attack，不再覆盖 Defense
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDef, EvaluateParameters, Attack);
    Attack = FMath::Max<float>(0.f, Attack);
    
    // 4. 抓取暴击率 (Source)
    float CritRate = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritRateDef, EvaluateParameters, CritRate);
    CritRate = FMath::Max<float>(0.f, CritRate);
    
    // 5. 初步融合：总伤害 = 基础伤害 + 面板攻击力
    float TotalDamage = BaseDamage + Attack;
    
    // 6. 暴击判定
    const bool bCriticalHit = FMath::RandRange(0.f, 100.f) < CritRate;
    if (bCriticalHit)
    {
        float CritDamage = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageDef, EvaluateParameters, CritDamage);
        CritDamage = FMath::Max<float>(1.f, CritDamage); // 保底是 1 倍
       
        // 直接乘！不除以 100 了
        TotalDamage *= CritDamage; 
    }
    
    // 7. 执行减伤公式
    if (Defense > 0.f)
    {
       // 🔴 修复 4：用 TotalDamage 进行运算，不再用 BaseDamage
       TotalDamage *= (100.f / (100.f + Defense));
    }
    
    // 8. 输出最终伤害
    // 🔴 修复 4：把算好的 TotalDamage 塞进去
    const FGameplayModifierEvaluatedData EvaluatedData(UPlayerAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, TotalDamage);
    OutExecutionOutput.AddOutputModifier(EvaluatedData);
    // 在最后一行 OutExecutionOutput 之前加上：
    UE_LOG(LogTemp, Warning, TEXT("【计算器生效了！】总算出的最终伤害是: %f"), TotalDamage);
}