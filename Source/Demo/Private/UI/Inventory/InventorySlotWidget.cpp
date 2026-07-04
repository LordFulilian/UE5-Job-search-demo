
#include "UI/Inventory/InventorySlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UInventorySlotWidget::InitializeSlot(FName InItemID, int32 InAmount, UTexture2D* InIcon)
{
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