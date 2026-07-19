

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
class UAnimMontage;

UCLASS(Abstract, BlueprintType, Blueprintable)
class DEMO_API ACharacterBase : public ACharacter, public IAbilitySystemInterface,public ICombatInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UFUNCTION(BlueprintPure, Category = "Ability")
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

	virtual  int32 GetPlayerLevel() override;
	virtual UAnimMontage * GetHitReactMontage_Implementation() override;
	
	virtual void Die() override;
	UFUNCTION(BlueprintPure, Category = "Death")
	bool IsDeathSequenceStarted() const { return bDeathSequenceStarted; }
	
	UFUNCTION(NetMulticast,Reliable)
	virtual void MulticasHamdleDeath();
protected:
	virtual void BeginPlay() override;
	

	// Weapon mesh attached to the character skeleton.
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
	virtual  void InitialzeDefaultAttributes() ;
	
	void AddCharacterAbilities() ;
	
	UPROPERTY(EditAnywhere,Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	bool bDeathSequenceStarted = false;
	bool bDeathPhysicsApplied = false;
	FTimerHandle DeathPhysicsFallbackTimer;

	void FinishDeathSequence();
	void ApplyDeathPhysics();
private:
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> HitReactAnimMontage;
};
