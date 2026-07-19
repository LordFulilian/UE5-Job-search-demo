// Rylan


#include "interaction/InteractionPromptWidget.h"

#include "Character/NPCCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "interaction/InteractableInterface.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"

void UInteractionPromptWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (DialogueButton)
	{
		DialogueButton->OnClicked.AddUniqueDynamic(
			this, &UInteractionPromptWidget::HandleDialogueButtonClicked);
	}
}

TSharedRef<SWidget> UInteractionPromptWidget::RebuildWidget()
{
	if (WidgetTree && WidgetTree->RootWidget)
	{
		return Super::RebuildWidget();
	}

	return SNew(SButton)
		.ContentPadding(FMargin(14.f, 8.f))
		.OnClicked_UObject(this, &UInteractionPromptWidget::HandleNativeButtonClicked)
		[
			SAssignNew(NativePromptText, STextBlock)
			.Text(NSLOCTEXT("Interaction", "DefaultTalkPrompt", "Talk"))
			.Font(FAppStyle::GetFontStyle("NormalFont"))
		];
}

void UInteractionPromptWidget::InitializePrompt(
	ANPCCharacter* InNPC,
	APlayerCharacter* InPlayer,
	const FText& InPromptText)
{
	NPC = InNPC;
	Player = InPlayer;
	SetPromptText(InPromptText);
}

void UInteractionPromptWidget::HandleDialogueButtonClicked()
{
	ANPCCharacter* NPCCharacter = NPC.Get();
	APlayerCharacter* PlayerCharacter = Player.Get();
	if (!IsValid(NPCCharacter) || !IsValid(PlayerCharacter) ||
		!IInteractableInterface::Execute_CanInteract(
			NPCCharacter, PlayerCharacter))
	{
		return;
	}

	IInteractableInterface::Execute_Interact(NPCCharacter, PlayerCharacter);
}

FReply UInteractionPromptWidget::HandleNativeButtonClicked()
{
	HandleDialogueButtonClicked();
	return FReply::Handled();
}

void UInteractionPromptWidget::SetPromptText(const FText& InPromptText)
{
	if (PromptText)
	{
		PromptText->SetText(InPromptText);
	}
	if (NativePromptText)
	{
		NativePromptText->SetText(InPromptText);
	}
}
