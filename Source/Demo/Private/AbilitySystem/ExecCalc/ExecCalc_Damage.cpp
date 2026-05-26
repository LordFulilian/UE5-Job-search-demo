// Rylan

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "PlayerAbilityTypes.h"
#include "PlayerGameplayTags.h"
#include "AbilitySystem/PlayerAbilitySystemLibrary.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "interaction/CombatInterface.h"

struct PlayerDamageStatics
{
    DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);
    DECLARE_ATTRIBUTE_CAPTUREDEF(Attack);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CritRate);
    DECLARE_ATTRIBUTE_CAPTUREDEF(CritDamage); 
    
    PlayerDamageStatics()
    {
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Defense, Target, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Attack, Source, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, CritRate, Source, false);
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
    
    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
    
    // 安全获取 Combat Interface
    ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
    ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
    
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
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDef, EvaluateParameters, Attack);
    Attack = FMath::Max<float>(0.f, Attack);
    
    // 4. 抓取暴击率 (Source)
    float CritRate = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritRateDef, EvaluateParameters, CritRate);
    CritRate = FMath::Max<float>(0.f, CritRate);
    
    // 5. 初步融合：总伤害 = 基础伤害 + 面板攻击力
    float TotalDamage = BaseDamage + Attack;
    
    // 6. 动态读取护甲系数 (安全检查防止指针崩溃)
    float DefenseCoefficient = 100.f; // 默认保底值
    if (SourceAvatar && SourceCombatInterface)
    {
        UCharacterClassInfo* CharacterClassInfo = UPlayerAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
        if (CharacterClassInfo && CharacterClassInfo->DamageCalculationCoefficients)
        {
            FRealCurve* DefenseCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("DefenseCoefficient"), FString());
            if (DefenseCurve)
            {
                // 获取与攻击者等级匹配的护甲系数
                DefenseCoefficient = DefenseCurve->Eval(SourceCombatInterface->GetPlayerLevel());
            }
        }
    }
    
    // 7. 暴击判定
    const bool bCriticalHit = FMath::RandRange(0.f, 100.f) < CritRate;
    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
    FGameplayEffectContext* Context = EffectContextHandle.Get();
    FPlayerGamePlayEffectContext* PlayerContext = static_cast<FPlayerGamePlayEffectContext*>(Context);
    PlayerContext->SetIsCriticalHit(bCriticalHit);
    
    if (bCriticalHit)
    {
        float CritDamage = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageDef, EvaluateParameters, CritDamage);
        CritDamage = FMath::Max<float>(1.f, CritDamage); // 保底倍率为 1.0
       
        // 执行暴击翻倍
        TotalDamage *= CritDamage; 
    }
    
    // 8. 执行减伤公式 (使用动态读取的 DefenseCoefficient)
    if (Defense > 0.f)
    {
       TotalDamage *= (DefenseCoefficient / (DefenseCoefficient + Defense));
    }
    
    // 9. 构造修改器，输出最终伤害给 AttributeSet
    const FGameplayModifierEvaluatedData EvaluatedData(UPlayerAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, TotalDamage);
    OutExecutionOutput.AddOutputModifier(EvaluatedData);
    
    // 10. 调试日志：确认计算器正确执行
    UE_LOG(LogTemp, Warning, TEXT("【GAS 伤害结算完毕】基础:%f, 攻击:%f, 暴击:%d, 防御:%f, 最终伤害:%f"), BaseDamage, Attack, bCriticalHit, Defense, TotalDamage);
}