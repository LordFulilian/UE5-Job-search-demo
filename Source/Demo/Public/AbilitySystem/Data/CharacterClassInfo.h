// Rylan

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayEffect;
class UGameplayAbility;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	longsword,
	shortsword,
	remote,
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly,Category="Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;
};
/**
 * 
 */
UCLASS()
class DEMO_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly,Category="Character Class Defaults")
	TMap<ECharacterClass,FCharacterClassDefaultInfo> CharacterClassInfomation;
	
	UPROPERTY(EditDefaultsOnly,Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;
	
	UPROPERTY(EditDefaultsOnly,Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	
	UPROPERTY(EditDefaultsOnly,Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;
	
	UPROPERTY(EditDefaultsOnly,Category="Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilites;
	
	FCharacterClassDefaultInfo CharacterClassDefaultInfo(ECharacterClass CharacterClass);
};
