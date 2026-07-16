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
class ABossCharacter;

UCLASS()
class DEMO_API AOnePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AOnePlayerController();

	// Toggles cursor visibility from the Alt input action.
	void ToggleMouseCursor();

	// Returns focus to the game when the player clicks the viewport.
	void OnClickScreen();

	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void OpenDialogue(UDialogueDataAsset* DialogueData, AActor* InteractionSource);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void CloseDialogue();
	void ShowBossHealthBar(ABossCharacter* Boss);
	void HideBossHealthBar();
	
	
	UFUNCTION(Client,Reliable)
	void ShowDamageNumber(float DamageAmount,ACharacter* TargetCharacter,bool bCriticalHit);
protected:
	virtual void BeginPlay() override;
	UPROPERTY(EditDefaultsOnly, Category = "Boss|UI")
	TSubclassOf<UPlayerUserWidget> BossHealthBarClass;

	UPROPERTY()
	TObjectPtr<UPlayerUserWidget> BossHealthBarInstance;

private:
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
	
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent>DamageTextComponentClass;
	
	
	
};
