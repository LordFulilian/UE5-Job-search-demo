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
       // Source attributes.
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Attack, Source, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, CritRate, Source, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, CritDamage, Source, false); 
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, EnergyRegen, Source, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, SkillDamageBonus, Source, false); 
       
       // Target attributes.
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, Defense, Target, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, FireResistance, Target, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, IceResistance, Target, false);
       DEFINE_ATTRIBUTE_CAPTUREDEF(UPlayerAttributeSet, PhysicalResistance, Target, false);
    }

    // Build this map lazily after native gameplay tags are initialized.
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

	if (TargetASC && TargetASC->HasMatchingGameplayTag(FPlayerGameplayTags::Get().State_Invulnerable))
	{
		return;
	}
    
    ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
    ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
    
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    
    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;
    
    // Sum each configured damage type after resistance mitigation.
    float BaseDamage = 0.f; 
    
    for (const TTuple<FGameplayTag, FGameplayTag>& Pair : FPlayerGameplayTags::Get().DamageTypesToResistances)
    {
        const FGameplayTag DamageTypeTag = Pair.Key;        
        const FGameplayTag ResistanceTag = Pair.Value;      

        checkf(DamageStatics().GetTagsToCaptureDefs().Contains(ResistanceTag), TEXT("TagsToCaptureDefs 字典里漏了标签: [%s]！"), *ResistanceTag.ToString());
        const FGameplayEffectAttributeCaptureDefinition CaptureDef = DamageStatics().GetTagsToCaptureDefs()[ResistanceTag];
        
        // Missing SetByCaller magnitudes contribute zero damage.
        float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag, false, 0.f);

        float Resistance = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParameters, Resistance);
        
        Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
        DamageTypeValue *= ( 100.f - Resistance ) / 100.f;

        BaseDamage += DamageTypeValue;
    }
    
    // Capture target defense.
    float Defense = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvaluateParameters, Defense);
    Defense = FMath::Max<float>(0.f, Defense);
    
    // Capture source attack.
    float Attack = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackDef, EvaluateParameters, Attack);
    Attack = FMath::Max<float>(0.f, Attack);
    
    // Capture source critical-hit rate.
    float CritRate = 0.f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritRateDef, EvaluateParameters, CritRate);
    CritRate = FMath::Max<float>(0.f, CritRate);
    
    // Scale mitigated base damage by the source attack percentage.
    // BaseDamage is the SetByCaller value (e.g. from curve table). Attack acts as a percentage bonus.
    float TotalDamage = BaseDamage * (1.0f + Attack / 100.0f);
     
    // Default defense coefficient (armor penetration factor)
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
    
    // Roll critical hits using a probability in the [0, 1] range.
    const bool bCriticalHit = FMath::RandRange(0.f, 1.f) < CritRate;
    FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
    FGameplayEffectContext* Context = EffectContextHandle.Get();
    FPlayerGamePlayEffectContext* PlayerContext = static_cast<FPlayerGamePlayEffectContext*>(Context);
    PlayerContext->SetIsCriticalHit(bCriticalHit);
    
    if (bCriticalHit)
    {
        float CritDamage = 0.f;
        ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CritDamageDef, EvaluateParameters, CritDamage);
        
        // Critical bonus damage cannot be negative.
        CritDamage = FMath::Max<float>(0.f, CritDamage); 
        
        // CritDamage is an additional percentage; 50 produces 150% total damage.
        TotalDamage += (TotalDamage * (CritDamage / 100.f)); 
    }
    
    // Apply defense mitigation.
    if (Defense > 0.f)
    {
       TotalDamage *= (DefenseCoefficient / (DefenseCoefficient + Defense));
    }
    
    // Send final damage through the IncomingDamage meta attribute.
    const FGameplayModifierEvaluatedData EvaluatedData(UPlayerAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, TotalDamage);
    OutExecutionOutput.AddOutputModifier(EvaluatedData);
    
}
