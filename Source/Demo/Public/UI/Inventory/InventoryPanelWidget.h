#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/PlayerUserWidget.h" 
#include "Components/ItemTypes.h"
#include "InventoryPanelWidget.generated.h"

class UWrapBox;
class UInventoryComponent;
class UInventorySlotWidget;
class UTextBlock;
class UImage;
class UButton; // 🔴 新增：按钮前向声明

UCLASS()
class DEMO_API UInventoryPanelWidget : public UPlayerUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void InitializePanel(UInventoryComponent* InInventoryComp);

    // 保留原本的全局刷新（比如刚打开背包时，默认刷新所有物品或指定分类）
    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void RefreshInventoryUI();

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void OnInventoryCloseButtonClicked();

    UFUNCTION()
    void UpdateDetailPanel(FName SelectedItemID);

protected:
    virtual void NativeConstruct() override; // 🔴 新增：用于绑定按钮事件

    // ==========================================
    // 🔴 新增：顶部的分类页签按钮 (名字需与蓝图一致)
    // ==========================================
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Btn_CategoryMaterial;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Btn_CategoryWeapon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Btn_CategoryQuest;

    // --- 原始 UI 控件 ---
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UWrapBox> Grid_Slots;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI")
    TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

    UPROPERTY()
    TObjectPtr<UInventoryComponent> InventoryReference;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> Text_DetailName;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UTextBlock> Text_DetailDesc;

    UPROPERTY(meta = (BindWidgetOptional))
    TObjectPtr<UImage> Img_DetailIcon;

private:
    // ==========================================
    // 🔴 新增：核心过滤刷新函数
    // ==========================================
    void RefreshInventoryByFilter(EItemType FilterType);

    UFUNCTION() void OnMaterialTabClicked();
    UFUNCTION() void OnWeaponTabClicked();
    UFUNCTION() void OnQuestTabClicked();
};