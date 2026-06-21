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
    DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(IceResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);
    DECLARE_ATTRIBUTE_CAPTUREDEF(EnergyRegen);
    DECLARE_ATTRIBUTE_CAPTUREDEF(SkillDamageBonus); 
    
    PlayerDamageStatics()
    {
       // --- 攻击方属性 (Source) ---
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Attack, Source, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, CritRate, Source, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, CritDamage, Source, false); 
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, EnergyRegen, Source, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, SkillDamageBonus, Source, false); 
       
       // --- 防御方属性 (Target) ---
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Defense, Target, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, FireResistance, Target, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, IceResistance, Target, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, PhysicalResistance, Target, false);
    }

    // 懒加载机制，确保标签单例初始化完成后再读取字典
    const TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition>& GetTagsToCaptureDefs() const
    {
        static TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
        
        if (TagsToCaptureDefs.IsEmpty())
        {
            const FPlayerGameplayTags& Tags = FPlayerGameplayTags::Get();
            TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CritDamage, CritDamageDef);
            TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CritRate, CritRateDef);
            TagsToCaptureDefs.Add(Tags.Attributes_Secondary_EnergyRegen, EnergyRegenDef);
            TagsToCaptureDefs.Add(Tags.Attributes_Primary_Attack, AttackDef);
            TagsToCaptureDefs.Add(Tags.Attributes_Primary_Defense, DefenseDef);
            TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
            TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Ice, IceResistanceDef);
            TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
            TagsToCaptureDefs.Add(Tags.Attributes_DamageBonus_SkillDamageBonus, SkillDamageBonusDef);
        }
        return TagsToCaptureDefs;
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
    RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef); 
    RelevantAttributesToCapture.Add(DamageStatics().IceResistanceDef); 
    RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef); 
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
    const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
    
    AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
    AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
    
    ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
    ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
    
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    
    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;
    
    // 1. 动态遍历多属性伤害，并计算元素抗性减免
    float BaseDamage = 0.f; 
    
    for (const TTuple<FGameplayTag, FGameplayTag>& Pair : FPlayerGameplayTags::Get().DamageTypesToResistances)
    {
        const FGameplayTag DamageTypeTag = Pair.Key;        
        const FGameplayTag ResistanceTag = Pair.Value;      

        checkf(DamageStatics().GetTagsToCaptureDefs().Contains(ResistanceTag), TEXT("TagsToCaptureDefs 字典里漏了标签: [%s]！"), *ResistanceTag.ToString());
        const FGameplayEffectAttributeCaptureDefinition CaptureDef = DamageStatics().GetTagsToCaptureDefs()[ResistanceTag];
        
        // 🔴 修复警告：传入 false 和 0.f 作为找不到标签时的默认处理，彻底消除红字报错
        float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false, 0.f);

        float Resistance = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance);
        
        Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
        DamageTypeValue *= ( 100.f - Resistance ) / 100.f;

        BaseDamage += DamageTypeValue;
    }
    
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
    
    UE_LOG(LogTemp, Warning, TEXT("【GAS 调试】成功抓取到 Source(攻击者) 的面板暴击率: %f"), CritRate);

    // 5. 初步融合：扣除抗性后的魔法伤害 + 面板物理攻击力
    float TotalDamage = BaseDamage + Attack;
    
    // 6. 动态读取护甲系数 
    float DefenseCoefficient = 100.f; 
    if (SourceAvatar && SourceCombatInterface)
    {
        UCharacterClassInfo* CharacterClassInfo = UPlayerAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
        if (CharacterClassInfo && CharacterClassInfo->DamageCalculationCoefficients)
        {
            FRealCurve* DefenseCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("DefenseCoefficient"), FString());
            if (DefenseCurve)
            {
                DefenseCoefficient = DefenseCurve->Eval(SourceCombatInterface->GetPlayerLevel());
            }
        }
    }
    
    // 7. 暴击判定 
    // 🔴 修复区间：按小数计算 (0.0 到 1.0)。填 0.05 就是 5% 的真实概率
    const bool bCriticalHit = FMath::RandRange(0.f, 1.f) < CritRate;
    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
    FGameplayEffectContext* Context = EffectContextHandle.Get();
    FPlayerGamePlayEffectContext* PlayerContext = static_cast<FPlayerGamePlayEffectContext*>(Context);
    PlayerContext->SetIsCriticalHit(bCriticalHit);
    
    if (bCriticalHit)
    {
        float CritDamage = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageDef, EvaluateParameters, CritDamage);
        
        // 🔴 流派 B 逻辑：保底不能是负增伤 (最低为 0)
        CritDamage = FMath::Max<float>(0.f, CritDamage); 
        
        // 🔴 流派 B 逻辑：总伤害 = 现有总伤害 + (现有总伤害 * (暴击伤害百分比 / 100))
        // 例如：暴击伤害填了 50，相当于加成 50%。100点伤害触发暴击后 = 100 + (100 * 0.5) = 150
        TotalDamage += (TotalDamage * (CritDamage / 100.f)); 
    }
    
    // 8. 执行减伤公式 
    if (Defense > 0.f)
    {
       TotalDamage *= (DefenseCoefficient / (DefenseCoefficient + Defense));
    }
    
    // 9. 构造修改器，输出最终伤害给 AttributeSet
    const FGameplayModifierEvaluatedData EvaluatedData(UPlayerAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, TotalDamage);
    OutExecutionOutput.AddOutputModifier(EvaluatedData);
    
    // 10. 调试日志
    UE_LOG(LogTemp, Warning, TEXT("【GAS 伤害结算】基础魔伤:%f, 攻击力:%f, 暴击率:%f, 是否暴击:%s, 目标防御:%f, 最终伤害:%f"), 
        BaseDamage, 
        Attack, 
        CritRate, 
        bCriticalHit ? TEXT("True(触发)") : TEXT("False(未触发)"), 
        Defense, 
        TotalDamage);
}