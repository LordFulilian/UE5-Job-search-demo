

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "GameFramework/Character.h"
#include "interaction/CombatInterface.h"
#include "CharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;

UCLASS(Abstract)
class DEMO_API ACharacterBase : public ACharacter, public IAbilitySystemInterface,public ICombatInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	virtual  int32 GetPlayerLevel() override;
protected:
	virtual void BeginPlay() override;
	

	// 武器骨骼网络
	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> Weapon;


	//
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent>AbilitySystemComponent; 

	//
	UPROPERTY()
	TObjectPtr<UAttributeSet>AttributeSet;
	
	virtual void InitAbilityActorInfo();
	
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Attributes")
	TSubclassOf<UGameplayEffect>DefaultPrimaryAttributes;
	
	UPROPERTY(BlueprintReadOnly,EditAnywhere,Category = "Attributes")
	TSubclassOf<UGameplayEffect>DefaultSecondaryAttributes;
	
	void ApplyEffectToSelf (TSubclassOf<UGameplayEffect>GameplayEffectClass,float Level) ;
	void InitializeDefaultAttributes() ;
	
	void AddCharacterAbilities() ;
private:
	
	UPROPERTY(EditAnywhere,Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

};
