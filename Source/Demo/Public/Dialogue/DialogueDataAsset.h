#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DialogueDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FDialogueLine
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue", meta = (MultiLine = true))
	FText Content;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<UTexture2D> Portrait;
};

UCLASS(BlueprintType)
class DEMO_API UDialogueDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TArray<FDialogueLine> Lines;
};
