// Rylan

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AttributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FAttributeInfo
{
	GENERATED_BODY()
    
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();
    
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FText AttributeName = FText();
    
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FText AttributeDescription = FText();
    
	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.0f;
};

/**
 * */
UCLASS()
class DEMO_API UAttributeInfoAsset : public UDataAsset 
{
	GENERATED_BODY()
    
public:
	FAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributesTag, bool bLogNotFound = false) const;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAttributeInfo> AttributeInformation;
};