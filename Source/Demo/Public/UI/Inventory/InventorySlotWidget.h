#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

// 前向声明 UI 组件
class UImage;
class UTextBlock;
class UButton;

// =========================================================================
// 🔴 1. 声明大喇叭类型：带 1 个 FName 参数，用来把被点击的物品 ID 喊出去
// =========================================================================
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClickedSignature, FName, ItemID);

UCLASS()
class DEMO_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 供外部面板调用的“数据注射”接口
	void InitializeSlot(FName InItemID, int32 InAmount, UTexture2D* InIcon);

	// =========================================================================
	// 🔴 2. 实例化这个大喇叭，名字必须叫 OnSlotClicked！主面板就是听它的！
	// =========================================================================
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnSlotClickedSignature OnSlotClicked;

protected:
	// UI 初始化生命周期函数
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Amount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Slot;

	// =========================================================================
	// 🔴 3. 核心通信变量与函数
	// =========================================================================
	// 用来记住当前格子里装的是什么物品
	UPROPERTY()
	FName CurrentItemID;

	// 按钮被按下时的响应函数
	UFUNCTION()
	void OnButtonClicked();
};