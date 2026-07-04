#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

// 前向声明 UI 组件
class UImage;
class UTextBlock;
class UButton;

UCLASS()
class DEMO_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 供外部面板调用的“数据注射”接口
	void InitializeSlot(FName InItemID, int32 InAmount, UTexture2D* InIcon);

protected:
	// ==========================================
	// 🔴 核心魔法：变量名必须与蓝图里的一模一样！
	// ==========================================
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Amount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Slot;
};