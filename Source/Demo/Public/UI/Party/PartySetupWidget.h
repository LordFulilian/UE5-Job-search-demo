#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/PlayerUserWidget.h"
#include "PartySetupWidget.generated.h"

class UPanelWidget;
class UPartyComponent;
class UPartyHeroCardWidget;
class UPartySetupSlotWidget;

/** Native controller for the Genshin-style party setup page. */
UCLASS(Abstract, Blueprintable)
class DEMO_API UPartySetupWidget : public UPlayerUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeWidgetControllerSet() override;
	virtual bool ShouldRunBlueprintWidgetControllerSet() const override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> ActivePartyBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> RosterBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Party|Setup")
	TSubclassOf<UPartySetupSlotWidget> PartySlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Party|Setup")
	TSubclassOf<UPartyHeroCardWidget> HeroCardWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "Party|Setup")
	int32 SelectedSlotIndex = 0;

private:
	UFUNCTION()
	void HandleNativePartyChanged();

	UFUNCTION()
	void HandleNativeRosterChanged();

	UFUNCTION()
	void HandleSlotSelected(int32 SlotIndex);

	UFUNCTION()
	void HandleHeroSelected(FGameplayTag HeroTag);

	void RebuildPartySlots();
	void RebuildRoster();
	void RefreshSlotSelection();
	void ApplyPageStyle();
	void UnbindPartyEvents();

	UPROPERTY(Transient)
	TObjectPtr<UPartyComponent> PartyComponent;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UPartySetupSlotWidget>> PartySlotWidgets;
};
