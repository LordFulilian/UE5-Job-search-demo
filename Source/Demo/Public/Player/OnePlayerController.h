// OnePlayerController.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "input/PlayerInputConfig.h"
#include "InputActionValue.h"
#include "OnePlayerController.generated.h"

class UDamageTextComponent;
class UInputMappingContext;
class UInputAction;
class UPlayerAbilitySystemComponent;
class UDialogueDataAsset;
class UDialogueWidget;
class UPlayerUserWidget;
class UUserWidget;
class ABossCharacter;

UCLASS(BlueprintType, Blueprintable)
class DEMO_API AOnePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOnePlayerController();

	// Toggles cursor visibility from the Alt input action.
	UFUNCTION(BlueprintCallable, Category = "Input")
	void ToggleMouseCursor();

	// Returns focus to the game when the player clicks the viewport.
	UFUNCTION(BlueprintCallable, Category = "Input")
	void OnClickScreen();

	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void OpenDialogue(UDialogueDataAsset* DialogueData, AActor* InteractionSource);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void CloseDialogue();

	UFUNCTION(BlueprintCallable, Category = "Quest|UI")
	void ToggleQuestList();

	UFUNCTION(BlueprintCallable, Category = "Party|UI")
	void TogglePartyPage();

	UFUNCTION(BlueprintCallable, Category = "Party|Switch")
	void SwitchToPartySlot(int32 SlotIndex);

	// Returns true when Widget belongs to a controller-managed menu.
	bool CloseManagedMenu(UPlayerUserWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "Boss|UI")
	void ShowBossHealthBar(ABossCharacter* Boss);

	UFUNCTION(BlueprintCallable, Category = "Boss|UI")
	void LeaveBossEncounter(ABossCharacter* Boss);

	void HideBossHealthBar();

	UFUNCTION(Client, Reliable)
	void ClientShowDeathScreen();

	UFUNCTION(BlueprintCallable, Category = "Death")
	void RequestRespawn();
	
	
	UFUNCTION(Client,Reliable)
	void ShowDamageNumber(float DamageAmount,ACharacter* TargetCharacter,bool bCriticalHit);
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category = "Boss|UI")
	TSubclassOf<UPlayerUserWidget> BossHealthBarClass;

	UPROPERTY()
	TObjectPtr<UPlayerUserWidget> BossHealthBarInstance;

	// Retained while the death screen is open so an active encounter can
	// restore its HUD after the player respawns.
	UPROPERTY()
	TWeakObjectPtr<ABossCharacter> ActiveBoss;

	UPROPERTY(EditDefaultsOnly, Category = "Death|UI")
	TSubclassOf<UUserWidget> DeathScreenClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> DeathScreenInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Quest|UI")
	TSubclassOf<UPlayerUserWidget> QuestListWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayerUserWidget> QuestListWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Party|UI")
	TSubclassOf<UPlayerUserWidget> PartyWidgetClass;

	UPROPERTY()
	TObjectPtr<UPlayerUserWidget> PartyWidgetInstance;

private:
	UFUNCTION(Server, Reliable)
	void ServerRequestRespawn();

	UFUNCTION(Server, Reliable)
	void ServerSwitchToPartySlot(int32 SlotIndex);

	UFUNCTION(Client, Reliable)
	void ClientHideDeathScreen();

	void HandleRespawn();
	void PerformPartySwitch(int32 SlotIndex);
	void SwitchToPartySlot1();
	void SwitchToPartySlot2();
	void SwitchToPartySlot3();
	void SwitchToPartySlot4();
	void OpenManagedMenu(UPlayerUserWidget* Widget, UObject* DataSource);
	void RestoreGameplayInput();

	UFUNCTION()
	void HandleDialogueFinished();

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;

	UPROPERTY()
	TObjectPtr<UDialogueWidget> ActiveDialogueWidget;

	UPROPERTY()
	TObjectPtr<AActor> ActiveInteractionSource;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerContext;

	// Alt-key input action.
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AltAction;

	// Left-click input action.
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ClickAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> QuestLogAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> PartyAction;
	
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent>DamageTextComponentClass;
	
	
	
};
