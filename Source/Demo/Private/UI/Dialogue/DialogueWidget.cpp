#include "UI/Dialogue/DialogueWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "Styling/AppStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"

void UDialogueWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (AdvanceButton)
	{
		AdvanceButton->OnClicked.AddDynamic(this, &UDialogueWidget::AdvanceDialogue);
	}
}

TSharedRef<SWidget> UDialogueWidget::RebuildWidget()
{
	if (WidgetTree && WidgetTree->RootWidget)
	{
		return Super::RebuildWidget();
	}

	return SNew(SOverlay)
		+ SOverlay::Slot()
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(64.f, 0.f, 64.f, 48.f))
		[
			SNew(SBorder)
			.Padding(24.f)
			.BorderImage(FAppStyle::GetBrush("Brushes.Panel"))
			.BorderBackgroundColor(FLinearColor(0.015f, 0.02f, 0.03f, 0.96f))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0.f, 0.f, 20.f, 0.f)
				[
					SNew(SBox)
					.WidthOverride(112.f)
					.HeightOverride(112.f)
					[
						SAssignNew(NativePortraitImage, SImage)
						.Visibility(EVisibility::Collapsed)
					]
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.f)
				[
					SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.f, 0.f, 0.f, 8.f)
					[
						SAssignNew(NativeSpeakerNameText, STextBlock)
						.Font(FAppStyle::GetFontStyle("HeadingMedium"))
						.ColorAndOpacity(FLinearColor(0.95f, 0.72f, 0.22f))
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.Padding(0.f, 0.f, 0.f, 16.f)
					[
						SAssignNew(NativeDialogueText, STextBlock)
						.Font(FAppStyle::GetFontStyle("NormalFont"))
						.AutoWrapText(true)
						.ColorAndOpacity(FLinearColor::White)
					]
					+ SVerticalBox::Slot()
					.AutoHeight()
					.HAlign(HAlign_Right)
					[
						SNew(SButton)
						.Text(NSLOCTEXT("Dialogue", "ContinueButton", "Continue"))
						.OnClicked_UObject(this, &UDialogueWidget::HandleNativeAdvanceClicked)
					]
				]
			]
		];
}

void UDialogueWidget::InitializeDialogue(UDialogueDataAsset* InDialogue)
{
	DialogueData = InDialogue;
	CurrentLineIndex = 0;
	if (!DialogueData || DialogueData->Lines.IsEmpty())
	{
		OnDialogueFinished.Broadcast();
		return;
	}
	ShowCurrentLine();
}

void UDialogueWidget::AdvanceDialogue()
{
	if (!DialogueData || !DialogueData->Lines.IsValidIndex(++CurrentLineIndex))
	{
		OnDialogueFinished.Broadcast();
		return;
	}
	ShowCurrentLine();
}

void UDialogueWidget::ShowCurrentLine()
{
	if (!DialogueData || !DialogueData->Lines.IsValidIndex(CurrentLineIndex)) return;

	const FDialogueLine& Line = DialogueData->Lines[CurrentLineIndex];
	if (SpeakerNameText) SpeakerNameText->SetText(Line.SpeakerName);
	if (DialogueText) DialogueText->SetText(Line.Content);
	if (NativeSpeakerNameText) NativeSpeakerNameText->SetText(Line.SpeakerName);
	if (NativeDialogueText) NativeDialogueText->SetText(Line.Content);
	if (PortraitImage)
	{
		PortraitImage->SetBrushFromTexture(Line.Portrait);
		PortraitImage->SetVisibility(Line.Portrait ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
	if (NativePortraitImage)
	{
		NativePortraitBrush.SetResourceObject(Line.Portrait);
		NativePortraitBrush.ImageSize = FVector2D(112.f, 112.f);
		NativePortraitImage->SetImage(&NativePortraitBrush);
		NativePortraitImage->SetVisibility(Line.Portrait ? EVisibility::Visible : EVisibility::Collapsed);
	}
	DialogueLineChanged(Line, CurrentLineIndex, CurrentLineIndex == DialogueData->Lines.Num() - 1);
}

FReply UDialogueWidget::HandleNativeAdvanceClicked()
{
	AdvanceDialogue();
	return FReply::Handled();
}
