#include "UI/Inventory/InventoryPanelWidget.h"
#include "Components/WrapBox.h"
#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/InventoryComponent.h" 
#include "Character/PlayerCharacter.h" 
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/ItemTypes.h"


// Bind category tabs when the widget is constructed.
void UInventoryPanelWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Btn_CategoryMaterial) Btn_CategoryMaterial->OnClicked.AddDynamic(this, &UInventoryPanelWidget::OnMaterialTabClicked);
    if (Btn_CategoryWeapon)   Btn_CategoryWeapon->OnClicked.AddDynamic(this, &UInventoryPanelWidget::OnWeaponTabClicked);
    if (Btn_CategoryQuest)    Btn_CategoryQuest->OnClicked.AddDynamic(this, &UInventoryPanelWidget::OnQuestTabClicked);
}

void UInventoryPanelWidget::InitializePanel(UInventoryComponent* InInventoryComp)
{
    if (!InInventoryComp) return;
    InventoryReference = InInventoryComp;
    
    // Show materials when the panel first opens.
    RefreshInventoryByFilter(EItemType::Material); 
}

// Category button callbacks select the active filter.
void UInventoryPanelWidget::OnMaterialTabClicked() { RefreshInventoryByFilter(EItemType::Material); }
void UInventoryPanelWidget::OnWeaponTabClicked()   { RefreshInventoryByFilter(EItemType::Weapon); }
void UInventoryPanelWidget::OnQuestTabClicked()    { RefreshInventoryByFilter(EItemType::Quest); }

// ==========================================
// Rebuild the grid using the selected category.
// ==========================================
void UInventoryPanelWidget::RefreshInventoryByFilter(EItemType FilterType)
{
    if (!InventoryReference || !Grid_Slots || !SlotWidgetClass) return;

    // Clear existing slot widgets.
    Grid_Slots->ClearChildren();

    // Iterate over runtime inventory slots.
    for (const auto& SlotData : InventoryReference->InventorySlots)
    {
       // Empty slots are not rendered.
       if (SlotData.ItemID == NAME_None || SlotData.Quantity <= 0) continue;

       FItemStaticData ItemData;
       if (InventoryReference->GetItemStaticData(SlotData.ItemID, ItemData))
       {
           // Create widgets only for items in the selected category.
           if (ItemData.ItemType == FilterType)
           {
               UInventorySlotWidget* NewSlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), SlotWidgetClass);
               if (NewSlotWidget)
               {
                   NewSlotWidget->InitializeSlot(SlotData.ItemID, SlotData.Quantity, ItemData.ItemIcon);
                   Grid_Slots->AddChildToWrapBox(NewSlotWidget);

                   // Preserve slot-click event forwarding.
                   NewSlotWidget->OnSlotClicked.AddDynamic(this, &UInventoryPanelWidget::UpdateDetailPanel);
               }
           }
       }
    }
}

// Default refresh retained for existing callers.
void UInventoryPanelWidget::RefreshInventoryUI()
{
    // Use the material category as the default view.
    RefreshInventoryByFilter(EItemType::Material);
}

void UInventoryPanelWidget::OnInventoryCloseButtonClicked()
{
    if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
    {
        PlayerChar->ToggleInventoryPanelAction(); 
    }
}

void UInventoryPanelWidget::UpdateDetailPanel(FName SelectedItemID)
{
    if (!InventoryReference) return;

    FItemStaticData ItemData;
    if (InventoryReference->GetItemStaticData(SelectedItemID, ItemData))
    {
        if (Text_DetailName) Text_DetailName->SetText(ItemData.ItemName);
        if (Img_DetailIcon && ItemData.ItemIcon)
        {
            Img_DetailIcon->SetBrushFromTexture(ItemData.ItemIcon);
            Img_DetailIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        }
        if (Text_DetailDesc) Text_DetailDesc->SetText(FText::FromString(TEXT("这是物品的详细描述信息..."))); 
    }
}
