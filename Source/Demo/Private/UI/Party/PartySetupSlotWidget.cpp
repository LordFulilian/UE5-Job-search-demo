#include "UI/Party/PartySetupSlotWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/RichTextBlock.h"

void UPartySetupSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (SlotButton)
	{
		SlotButton->SetBackgroundColor(FLinearColor(0.045f, 0.06f, 0.1f, 0.92f));
		// Replace legacy Blueprint click bindings. The page owns party mutation.
		SlotButton->OnClicked.Clear();
		SlotButton->OnClicked.AddUniqueDynamic(
			this, &UPartySetupSlotWidget::HandleSlotClicked);
	}

	if (SlotBackground)
	{
		SlotBackground->SetBrushColor(FLinearColor(0.035f, 0.055f, 0.095f, 0.88f));
		if (UOverlaySlot* BackgroundSlot = Cast<UOverlaySlot>(SlotBackground->Slot))
		{
			BackgroundSlot->SetHorizontalAlignment(HAlign_Fill);
			BackgroundSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
	if (FullBodyImage)
	{
		if (UOverlaySlot* ImageSlot = Cast<UOverlaySlot>(FullBodyImage->Slot))
		{
			ImageSlot->SetHorizontalAlignment(HAlign_Fill);
			ImageSlot->SetVerticalAlignment(VAlign_Fill);
			ImageSlot->SetPadding(FMargin(120.f, 4.f, 8.f, 4.f));
		}
	}
	if (EmptyPlusText)
	{
		EmptyPlusText->SetDefaultColorAndOpacity(FSlateColor(FLinearColor::White));
		if (UOverlaySlot* PlusSlot = Cast<UOverlaySlot>(EmptyPlusText->Slot))
		{
			PlusSlot->SetHorizontalAlignment(HAlign_Center);
			PlusSlot->SetVerticalAlignment(VAlign_Center);
		}
	}
	if (HeroInfoPanel)
	{
		if (UOverlaySlot* InfoSlot = Cast<UOverlaySlot>(HeroInfoPanel->Slot))
		{
			InfoSlot->SetHorizontalAlignment(HAlign_Left);
			InfoSlot->SetVerticalAlignment(VAlign_Bottom);
			InfoSlot->SetPadding(FMargin(18.f, 8.f, 8.f, 14.f));
		}
	}
	if (HeroNameText)
	{
		HeroNameText->SetDefaultColorAndOpacity(FSlateColor(FLinearColor::White));
	}
	if (HeroLevelText)
	{
		HeroLevelText->SetDefaultColorAndOpacity(
			FSlateColor(FLinearColor(0.8f, 0.86f, 1.f, 1.f)));
	}
	if (SelectedFrame)
	{
		SelectedFrame->SetBrushColor(FLinearColor(0.95f, 0.75f, 0.24f, 1.f));
		if (UOverlaySlot* FrameSlot = Cast<UOverlaySlot>(SelectedFrame->Slot))
		{
			FrameSlot->SetHorizontalAlignment(HAlign_Fill);
			FrameSlot->SetVerticalAlignment(VAlign_Fill);
			FrameSlot->SetPadding(FMargin(2.f));
		}
	}
}

void UPartySetupSlotWidget::ApplySlotData(
	const FPartySlotViewData& InSlotData)
{
	SlotData = InSlotData;
	RefreshPresentation();
}

void UPartySetupSlotWidget::SetSelected(bool bInSelected)
{
	if (SelectedFrame)
	{
		SelectedFrame->SetVisibility(bInSelected
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	}
}

void UPartySetupSlotWidget::HandleSlotClicked()
{
	OnPartySlotSelected.Broadcast(SlotData.SlotIndex);
}

void UPartySetupSlotWidget::RefreshPresentation()
{
	const bool bOccupied = SlotData.bOccupied;
	const ESlateVisibility OccupiedVisibility = bOccupied
		? ESlateVisibility::SelfHitTestInvisible
		: ESlateVisibility::Collapsed;

	if (HeroInfoPanel) HeroInfoPanel->SetVisibility(OccupiedVisibility);
	if (EmptyPlusText)
	{
		EmptyPlusText->SetVisibility(bOccupied
			? ESlateVisibility::Collapsed
			: ESlateVisibility::SelfHitTestInvisible);
	}

	if (FullBodyImage)
	{
		UTexture2D* DisplayTexture = nullptr;
		if (bOccupied)
		{
			DisplayTexture = const_cast<UTexture2D*>(
				SlotData.HeroInfo.FullBodyArtwork.Get());
			if (!DisplayTexture)
			{
				DisplayTexture = const_cast<UTexture2D*>(
					SlotData.HeroInfo.AvatarIcon.Get());
			}
		}
		FullBodyImage->SetVisibility(OccupiedVisibility);
		FullBodyImage->SetBrushFromTexture(DisplayTexture);
	}

	if (ElementIconImage)
	{
		UTexture2D* ElementTexture = bOccupied
			? const_cast<UTexture2D*>(SlotData.HeroInfo.ElementIcon.Get())
			: nullptr;
		ElementIconImage->SetBrushFromTexture(ElementTexture);
		ElementIconImage->SetVisibility(ElementTexture
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	}

	if (HeroNameText)
	{
		HeroNameText->SetText(bOccupied
			? SlotData.HeroInfo.HeroName
			: FText::GetEmpty());
	}
	if (HeroLevelText)
	{
		HeroLevelText->SetText(bOccupied
			? FText::Format(NSLOCTEXT("Party", "HeroLevel", "Lv.{0}"),
				FText::AsNumber(SlotData.HeroInfo.DisplayLevel))
			: FText::GetEmpty());
	}
}
