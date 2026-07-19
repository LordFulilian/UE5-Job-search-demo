// Rylan

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Input/Reply.h"
#include "InteractionPromptWidget.generated.h"

class ANPCCharacter;
class APlayerCharacter;
class UButton;
class UTextBlock;
class STextBlock;

UCLASS()
class DEMO_API UInteractionPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializePrompt(
		ANPCCharacter* InNPC,
		APlayerCharacter* InPlayer,
		const FText& InPromptText);

protected:
	virtual void NativeOnInitialized() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> DialogueButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PromptText;

private:
	UFUNCTION()
	void HandleDialogueButtonClicked();

	FReply HandleNativeButtonClicked();
	void SetPromptText(const FText& InPromptText);

	UPROPERTY(Transient)
	TWeakObjectPtr<ANPCCharacter> NPC;

	UPROPERTY(Transient)
	TWeakObjectPtr<APlayerCharacter> Player;

	TSharedPtr<STextBlock> NativePromptText;
};
