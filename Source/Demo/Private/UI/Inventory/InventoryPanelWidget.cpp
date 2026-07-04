#include "UI/Inventory/InventoryPanelWidget.h"
#include "Components/WrapBox.h"
#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/InventoryComponent.h" 

void UInventoryPanelWidget::InitializePanel(UInventoryComponent* InInventoryComp)
{
	if (!InInventoryComp) return;

	InventoryReference = InInventoryComp;
    
	// 绑定委托：当背包数据改变时，自动触发 RefreshInventoryUI (需要你的组件里有这个委托)
	// InventoryReference->OnInventoryUpdated.AddDynamic(this, &UInventoryPanelWidget::RefreshInventoryUI);

	RefreshInventoryUI();
}

void UInventoryPanelWidget::RefreshInventoryUI()
{
	if (!InventoryReference || !Grid_Slots || !SlotWidgetClass) return;

	// 1. 清空旧的 UI 格子
	Grid_Slots->ClearChildren();

	// 2. 遍历真实的后台背包数据 (假设你的背包组件里有一个叫 InventorySlots 的数组)
	for (const auto& SlotData : InventoryReference->InventorySlots)
	{
		// 跳过空数据
		if (SlotData.ItemID == NAME_None || SlotData.Quantity <= 0) continue;

		// 3. 动态创建 C++ 格子实例
		UInventorySlotWidget* NewSlotWidget = CreateWidget<UInventorySlotWidget>(GetWorld(), SlotWidgetClass);
		if (NewSlotWidget)
		{
			// 4. 从数据表中读取真实的图标 (假设你在 InventoryComponent 写过这个辅助函数)
			UTexture2D* ItemIcon = nullptr; 
			FItemStaticData ItemData;
			if (InventoryReference->GetItemStaticData(SlotData.ItemID, ItemData))
			{
				ItemIcon = ItemData.ItemIcon;
			}

			// 5. 注入数据！
			NewSlotWidget->InitializeSlot(SlotData.ItemID, SlotData.Quantity, ItemIcon);

			// 6. 塞进网格里
			Grid_Slots->AddChildToWrapBox(NewSlotWidget);
		}
	}
}