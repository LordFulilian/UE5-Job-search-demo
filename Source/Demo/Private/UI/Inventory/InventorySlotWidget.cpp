#include "UI/Inventory/InventorySlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
// 🔴 记得引入 Button 头文件
#include "Components/Button.h" 

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 🔴 绑定那个透明按钮的点击事件
	if (Btn_Slot)
	{
		Btn_Slot->OnClicked.AddDynamic(this, &UInventorySlotWidget::OnButtonClicked);
	}
}

void UInventorySlotWidget::InitializeSlot(FName InItemID, int32 InAmount, UTexture2D* InIcon)
{
	// 🔴 极其关键：把主面板传给我的 ID 悄悄存在自己肚子里
	CurrentItemID = InItemID;

	// 1. 设置数量
	if (Text_Amount)
	{
		Text_Amount->SetText(FText::AsNumber(InAmount));
	}

	// 2. 设置图标
	if (Img_Icon && InIcon)
	{
		Img_Icon->SetBrushFromTexture(InIcon);
		// 设置可见性，防止没图片的格子显示白块
		Img_Icon->SetVisibility(ESlateVisibility::SelfHitTestInvisible); 
	}
}

// =========================================================================
// 🔴 按钮被点击后的具体表现
// =========================================================================
void UInventorySlotWidget::OnButtonClicked()
{
	// 如果我这个格子不是空的
	if (CurrentItemID != NAME_None)
	{
		// 拿起大喇叭广播：“我被点啦！我的 ID 是 CurrentItemID！”
		OnSlotClicked.Broadcast(CurrentItemID);
	}
}