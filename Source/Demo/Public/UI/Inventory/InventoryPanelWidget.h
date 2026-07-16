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
class UButton;

UCLASS()
class DEMO_API UInventoryPanelWidget : public UPlayerUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void InitializePanel(UInventoryComponent* InInventoryComp);

    // Rebuilds the inventory using an optional category filter.
    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void RefreshInventoryUI();

    UFUNCTION(BlueprintCallable, Category = "Inventory UI")
    void OnInventoryCloseButtonClicked();

    UFUNCTION()
    void UpdateDetailPanel(FName SelectedItemID);

protected:
    virtual void NativeConstruct() override;

    // ==========================================
    // Category tabs bound by matching Designer names.
    // ==========================================
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Btn_CategoryMaterial;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Btn_CategoryWeapon;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UButton> Btn_CategoryQuest;

    // Inventory controls.
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
    // Applies a category filter and rebuilds visible slots.
    // ==========================================
    void RefreshInventoryByFilter(EItemType FilterType);

    UFUNCTION() void OnMaterialTabClicked();
    UFUNCTION() void OnWeaponTabClicked();
    UFUNCTION() void OnQuestTabClicked();
};
