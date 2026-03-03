

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;

UCLASS(Abstract)
class DEMO_API ACharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }

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
	
	void InitialzePrimaryAttributes() const;

};
