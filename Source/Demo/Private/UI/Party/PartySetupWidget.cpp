#include "UI/Party/PartySetupWidget.h"

#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/RichTextBlock.h"
#include "Components/SizeBox.h"
#include "Components/PartyComponent.h"
#include "Blueprint/WidgetTree.h"
#include "UI/Party/PartyHeroCardWidget.h"
#include "UI/Party/PartySetupSlotWidget.h"

bool UPartySetupWidget::ShouldRunBlueprintWidgetControllerSet() const
{
	// The legacy Blueprint graph rebuilds only occupied entries and binds old
	// Add/Remove semantics. Native code owns the page data flow now.
	return false;
}

void UPartySetupWidget::NativeWidgetControllerSet()
{
	ApplyPageStyle();
	UnbindPartyEvents();
	PartyComponent = Cast<UPartyComponent>(WidgetController);
	if (!PartyComponent) return;

	PartyComponent->OnPartyChanged.AddUniqueDynamic(
		this, &UPartySetupWidget::HandleNativePartyChanged);
	PartyComponent->OnRosterChanged.AddUniqueDynamic(
		this, &UPartySetupWidget::HandleNativeRosterChanged);

	RebuildPartySlots();
	RebuildRoster();
}

void UPartySetupWidget::ApplyPageStyle()
{
	if (!WidgetTree) return;

	// The Designer background is intentionally texture-agnostic.  Tinting it
	// here keeps the world visible while giving all party controls contrast.
	if (UImage* Background = Cast<UImage>(WidgetTree->FindWidget(TEXT("Image"))))
	{
		Background->SetColorAndOpacity(FLinearColor(0.025f, 0.04f, 0.075f, 0.82f));
	}

	TArray<UWidget*> PageWidgets;
	WidgetTree->GetAllWidgets(PageWidgets);
	for (UWidget* Widget : PageWidgets)
	{
		if (URichTextBlock* Text = Cast<URichTextBlock>(Widget))
		{
			Text->SetDefaultColorAndOpacity(FSlateColor(FLinearColor::White));
		}
	}
}

void UPartySetupWidget::NativeDestruct()
{
	UnbindPartyEvents();
	Super::NativeDestruct();
}

void UPartySetupWidget::UnbindPartyEvents()
{
	if (PartyComponent)
	{
		PartyComponent->OnPartyChanged.RemoveDynamic(
			this, &UPartySetupWidget::HandleNativePartyChanged);
		PartyComponent->OnRosterChanged.RemoveDynamic(
			this, &UPartySetupWidget::HandleNativeRosterChanged);
	}
	PartyComponent = nullptr;
}

void UPartySetupWidget::HandleNativePartyChanged()
{
	RebuildPartySlots();
	RebuildRoster();
}

void UPartySetupWidget::HandleNativeRosterChanged()
{
	RebuildRoster();
}

void UPartySetupWidget::HandleSlotSelected(int32 SlotIndex)
{
	SelectedSlotIndex = SlotIndex;
	RefreshSlotSelection();
}

void UPartySetupWidget::HandleHeroSelected(FGameplayTag HeroTag)
{
	if (PartyComponent)
	{
		PartyComponent->SetPartySlot(SelectedSlotIndex, HeroTag);
	}
}

void UPartySetupWidget::RebuildPartySlots()
{
	if (!PartyComponent || !ActivePartyBox || !PartySlotWidgetClass) return;

	ActivePartyBox->ClearChildren();
	PartySlotWidgets.Reset();
	const TArray<FPartySlotViewData> Slots = PartyComponent->GetPartySlots();
	if (!Slots.IsValidIndex(SelectedSlotIndex)) SelectedSlotIndex = 0;

	for (const FPartySlotViewData& SlotData : Slots)
	{
		UPartySetupSlotWidget* SlotWidget =
			CreateWidget<UPartySetupSlotWidget>(
				GetOwningPlayer(), PartySlotWidgetClass);
		if (!SlotWidget) continue;

		SlotWidget->ApplySlotData(SlotData);
		SlotWidget->OnPartySlotSelected.AddUniqueDynamic(
			this, &UPartySetupWidget::HandleSlotSelected);
		// The legacy Widget Blueprint was authored on a 1280x720 canvas.  A
		// SizeBox prevents that preview canvas from making every party entry
		// consume an entire screen when it is inserted into the vertical list.
		USizeBox* SlotSizeBox = NewObject<USizeBox>(this);
		SlotSizeBox->SetHeightOverride(140.f);
		SlotSizeBox->SetContent(SlotWidget);
		ActivePartyBox->AddChild(SlotSizeBox);
		PartySlotWidgets.Add(SlotWidget);
	}
	RefreshSlotSelection();
}

void UPartySetupWidget::RebuildRoster()
{
	if (!PartyComponent || !RosterBox || !HeroCardWidgetClass) return;

	RosterBox->ClearChildren();
	for (const FHeroSlotInfo& HeroInfo :
		PartyComponent->GetUnlockedHeroInfo())
	{
		UPartyHeroCardWidget* HeroCard =
			CreateWidget<UPartyHeroCardWidget>(
				GetOwningPlayer(), HeroCardWidgetClass);
		if (!HeroCard) continue;

		HeroCard->ApplyHeroData(HeroInfo, PartyComponent);
		HeroCard->OnHeroSelected.AddUniqueDynamic(
			this, &UPartySetupWidget::HandleHeroSelected);
		// Roster cards use the same full-screen Designer preview, so constrain
		// their desired size before adding them to the wrap/grid container.
		USizeBox* CardSizeBox = NewObject<USizeBox>(this);
		CardSizeBox->SetWidthOverride(150.f);
		CardSizeBox->SetHeightOverride(180.f);
		CardSizeBox->SetContent(HeroCard);
		RosterBox->AddChild(CardSizeBox);
	}
}

void UPartySetupWidget::RefreshSlotSelection()
{
	for (UPartySetupSlotWidget* SlotWidget : PartySlotWidgets)
	{
		if (SlotWidget)
		{
			SlotWidget->SetSelected(
				SlotWidget->GetSlotIndex() == SelectedSlotIndex);
		}
	}
}
