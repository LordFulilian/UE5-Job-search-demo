

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h" 
#include "PlayerAttributeSet.generated.h"



#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()
	FEffectProperties(){}
	
	FGameplayEffectContextHandle EffectContextHandle;
	
	UPROPERTY()
	UAbilitySystemComponent *SourceASC = nullptr;
	
	UPROPERTY()
	AActor * SourceAvatarActor = nullptr;
	
	UPROPERTY()
	AController* SourceController = nullptr;
	
	UPROPERTY()
	ACharacter * SourceCharacter= nullptr;
	
	UPROPERTY()
	UAbilitySystemComponent *TargetASC = nullptr;
	
	UPROPERTY()
	AActor * TargetAvatarActor = nullptr;
	
	UPROPERTY()
	AController* TargetController = nullptr;
	
	UPROPERTY()
	ACharacter * TargetCharacter = nullptr;
	
};
/**
 * 
 */
UCLASS()
class DEMO_API UPlayerAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:	
	UPlayerAttributeSet();

	virtual	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	// Health
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Health);
	
	
	
	// 1. 攻击力 (Attack)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Attack, Category = "Primary Attributes")
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Attack);

	

	// 2. 防御力 (Defense)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Defense, Category = "Primary Attributes")
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Defense);

	// 3. 暴击率 (Crit Rate) - 通常以百分比表示，比如 0.05 代表 5%
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritRate, Category = "Primary Attributes")
	FGameplayAttributeData CritRate;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CritRate);

	// 4. 暴击伤害 (Crit Damage) - 通常初始值为 1.5 代表 150% 爆伤
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CritDamage, Category = "Primary Attributes")
	FGameplayAttributeData CritDamage;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, CritDamage);
	
	// Max Health
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxHealth);

	UFUNCTION()
    	void OnRep_Health(const FGameplayAttributeData& OldHealth)	const;
	
	UFUNCTION()
    	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)const;
	
	UFUNCTION()
	void OnRep_Attack(const FGameplayAttributeData& OldAttack)	const;
	
	UFUNCTION()
	void OnRep_Defense(const FGameplayAttributeData& OldDefense)const;
	
	UFUNCTION()
	void OnRep_CritRate(const FGameplayAttributeData& OldCritRate)const;
	
	UFUNCTION()
	void OnRep_CritDamage(const FGameplayAttributeData& OldCritDamage)const;
    
	


private:
	void SetEffectProperties (const struct FGameplayEffectModCallbackData& Data,FEffectProperties& Props)const;
	
};
