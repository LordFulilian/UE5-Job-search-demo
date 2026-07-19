#include "UI/Party/PartyHeroCardWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/OverlaySlot.h"
#include "Components/PartyComponent.h"
#include "Components/RichTextBlock.h"

void UPartyHeroCardWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (HeroButton)
	{
		HeroButton->SetBackgroundColor(FLinearColor(0.055f, 0.075f, 0.12f, 0.94f));
		// Discard the legacy Add/Remove binding from WBP_HeroCard.
		HeroButton->OnClicked.Clear();
		HeroButton->OnClicked.AddUniqueDynamic(
			this, &UPartyHeroCardWidget::HandleHeroClicked);
	}

	if (HeroAvatarImage)
	{
		if (UOverlaySlot* AvatarSlot = Cast<UOverlaySlot>(HeroAvatarImage->Slot))
		{
			AvatarSlot->SetHorizontalAlignment(HAlign_Fill);
			AvatarSlot->SetVerticalAlignment(VAlign_Fill);
			AvatarSlot->SetPadding(FMargin(6.f, 6.f, 6.f, 28.f));
		}
	}
	if (HeroNameText)
	{
		HeroNameText->SetDefaultColorAndOpacity(FSlateColor(FLinearColor::White));
		if (UOverlaySlot* NameSlot = Cast<UOverlaySlot>(HeroNameText->Slot))
		{
			NameSlot->SetHorizontalAlignment(HAlign_Center);
			NameSlot->SetVerticalAlignment(VAlign_Bottom);
			NameSlot->SetPadding(FMargin(4.f, 0.f, 4.f, 5.f));
		}
	}
	if (PartyStateText)
	{
		PartyStateText->SetDefaultColorAndOpacity(
			FSlateColor(FLinearColor(0.75f, 0.95f, 1.f, 1.f)));
		if (UOverlaySlot* StateSlot = Cast<UOverlaySlot>(PartyStateText->Slot))
		{
			StateSlot->SetHorizontalAlignment(HAlign_Left);
			StateSlot->SetVerticalAlignment(VAlign_Top);
			StateSlot->SetPadding(FMargin(8.f));
		}
	}
}

void UPartyHeroCardWidget::ApplyHeroData(
	const FHeroSlotInfo& InHeroInfo, UPartyComponent* InPartyComponent)
{
	HeroInfo = InHeroInfo;
	PartyComponent = InPartyComponent;

	if (HeroAvatarImage)
	{
		HeroAvatarImage->SetBrushFromTexture(
			const_cast<UTexture2D*>(HeroInfo.AvatarIcon.Get()));
	}
	if (HeroNameText) HeroNameText->SetText(HeroInfo.HeroName);
	if (PartyStateText)
	{
		const bool bInParty = PartyComponent &&
			PartyComponent->IsHeroInParty(HeroInfo.HeroTag);
		PartyStateText->SetText(bInParty
			? NSLOCTEXT("Party", "InParty", "In Party")
			: FText::GetEmpty());
		PartyStateText->SetVisibility(bInParty
			? ESlateVisibility::SelfHitTestInvisible
			: ESlateVisibility::Collapsed);
	}
}

void UPartyHeroCardWidget::HandleHeroClicked()
{
	if (HeroInfo.HeroTag.IsValid())
	{
		OnHeroSelected.Broadcast(HeroInfo.HeroTag);
	}
}
