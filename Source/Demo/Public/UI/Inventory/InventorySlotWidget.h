#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

// Forward declarations for bound UI controls.
class UImage;
class UTextBlock;
class UButton;

// =========================================================================
// Reports the item identifier for a clicked slot.
// =========================================================================
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotClickedSignature, FName, ItemID);

UCLASS()
class DEMO_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Populates the slot with item data.
	void InitializeSlot(FName InItemID, int32 InAmount, UTexture2D* InIcon);

	// =========================================================================
	// Broadcast when the slot button is clicked.
	// =========================================================================
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnSlotClickedSignature OnSlotClicked;

protected:
	// Binds the slot button.
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Amount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Slot;

	// =========================================================================
	// Current slot state.
	// =========================================================================
	// Item represented by this slot.
	UPROPERTY()
	FName CurrentItemID;

	// Handles button clicks and broadcasts the item identifier.
	UFUNCTION()
	void OnButtonClicked();
};
