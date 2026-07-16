#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dialogue/DialogueDataAsset.h"
#include "Input/Reply.h"
#include "Styling/SlateBrush.h"
#include "DialogueWidget.generated.h"

class UButton;
class UDialogueDataAsset;
class UImage;
class UTextBlock;
class SImage;
class STextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDialogueFinishedSignature);

UCLASS()
class DEMO_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void InitializeDialogue(UDialogueDataAsset* InDialogue);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AdvanceDialogue();

	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FDialogueFinishedSignature OnDialogueFinished;

protected:
	virtual void NativeOnInitialized() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void DialogueLineChanged(const FDialogueLine& Line, int32 LineIndex, bool bIsLastLine);

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SpeakerNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DialogueText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> PortraitImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> AdvanceButton;

private:
	void ShowCurrentLine();
	FReply HandleNativeAdvanceClicked();

	UPROPERTY()
	TObjectPtr<UDialogueDataAsset> DialogueData;

	int32 CurrentLineIndex = INDEX_NONE;

	TSharedPtr<STextBlock> NativeSpeakerNameText;
	TSharedPtr<STextBlock> NativeDialogueText;
	TSharedPtr<SImage> NativePortraitImage;
	FSlateBrush NativePortraitBrush;
};
