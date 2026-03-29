// Rylan

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AttributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FPlayerAttributeInfo
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsPercentage = false;
};

/**
 * */
UCLASS()
class DEMO_API UAttributeInfo : public UDataAsset 
{
	GENERATED_BODY()
    
public:
	FPlayerAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributesTag, bool bLogNotFound = false) const;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FPlayerAttributeInfo> AttributeInformation;
	
	
};