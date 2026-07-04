#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/PlayerUserWidget.h" 
#include "InventoryPanelWidget.generated.h"

class UWrapBox;
class UInventoryComponent;
class UInventorySlotWidget;

UCLASS()
// 2. 🔴 这里改为继承自 UPlayerUserWidget
class DEMO_API UInventoryPanelWidget : public UPlayerUserWidget
{
	GENERATED_BODY()

public:
	// 如果你的 PlayerUserWidget 里有一个类似 WidgetControllerSet 的虚函数，
	// 你可以直接重写它，在里面完成背包数据的绑定！
    
	// (保留之前的这些声明)
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void InitializePanel(UInventoryComponent* InInventoryComp);

	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void RefreshInventoryUI();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> Grid_Slots;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory UI")
	TSubclassOf<UInventorySlotWidget> SlotWidgetClass;

	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryReference;
};