#include "UI/Inventory/InventoryPanelWidget.h"
#include "Components/WrapBox.h"
#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/InventoryComponent.h" 
#include "Character/PlayerCharacter.h" 
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h" // 🔴 新增按钮头文件
#include "Components/ItemTypes.h"


// 🔴 新增：在这里绑定按钮点击事件
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
    
    // 打开面板时，默认展示“材料”分类
    RefreshInventoryByFilter(EItemType::Material); 
}

// 按钮点击回调：传递不同的过滤参数
void UInventoryPanelWidget::OnMaterialTabClicked() { RefreshInventoryByFilter(EItemType::Material); }
void UInventoryPanelWidget::OnWeaponTabClicked()   { RefreshInventoryByFilter(EItemType::Weapon); }
void UInventoryPanelWidget::OnQuestTabClicked()    { RefreshInventoryByFilter(EItemType::Quest); }

// ==========================================
// 🔴 核心升级：带有过滤功能的网格刷新逻辑
// ==========================================
void UInventoryPanelWidget::RefreshInventoryByFilter(EItemType FilterType)
{
    if (!InventoryReference || !Grid_Slots || !SlotWidgetClass) return;

    // 1. 清空当前网格
    Grid_Slots->ClearChildren();

    // 2. 遍历背包数据
    for (const auto& SlotData : InventoryReference->InventorySlots)
    {
       // 如果是空格子，跳过不渲染
       if (SlotData.ItemID == NAME_None || SlotData.Quantity <= 0) continue;

       FItemStaticData ItemData;
       if (InventoryReference->GetItemStaticData(SlotData.ItemID, ItemData))
       {
           // 🔴 3. 最关键的一步：只有物品类型匹配当前页签，才允许生成格子！
           if (ItemData.ItemType == FilterType)
           {
               UInventorySlotWidget* NewSlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), SlotWidgetClass);
               if (NewSlotWidget)
               {
                   NewSlotWidget->InitializeSlot(SlotData.ItemID, SlotData.Quantity, ItemData.ItemIcon);
                   Grid_Slots->AddChildToWrapBox(NewSlotWidget);

                   // 依然保留格子点击事件的绑定
                   NewSlotWidget->OnSlotClicked.AddDynamic(this, &UInventoryPanelWidget::UpdateDetailPanel);
               }
           }
       }
    }
}

// (原来的 RefreshInventoryUI 可以保留以防其他地方调用，但逻辑也可以指向 Filter)
void UInventoryPanelWidget::RefreshInventoryUI()
{
    // 如果没有指定，默认刷新材料
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