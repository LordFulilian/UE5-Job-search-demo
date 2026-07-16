#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind the transparent slot button.
	if (Btn_Slot)
	{
		Btn_Slot->OnClicked.AddDynamic(this, &UInventorySlotWidget::OnButtonClicked);
	}
}

void UInventorySlotWidget::InitializeSlot(FName InItemID, int32 InAmount, UTexture2D* InIcon)
{
	// Cache the item identifier for click events.
	CurrentItemID = InItemID;

	// Update the stack count.
	if (Text_Amount)
	{
		Text_Amount->SetText(FText::AsNumber(InAmount));
	}

	// Update the item icon.
	if (Img_Icon && InIcon)
	{
		Img_Icon->SetBrushFromTexture(InIcon);
		// Reveal the image only when a texture exists.
		Img_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible); 
	}
}

// =========================================================================
// Forward valid slot clicks to the inventory panel.
// =========================================================================
void UInventorySlotWidget::OnButtonClicked()
{
	// Ignore empty slots.
	if (CurrentItemID != NAME_None)
	{
		// Broadcast the selected item identifier.
		OnSlotClicked.Broadcast(CurrentItemID);
	}
}
