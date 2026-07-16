// Rylan

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

UCLASS()
class DEMO_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Gameplay level loaded when the player starts the game.
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSoftObjectPtr<UWorld> GameplayLevel;

protected:
	virtual void NativeConstruct() override;

	// Automatically bound to the start-game button in the Widget Blueprint.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_EnterGame;

	// Automatically bound to the quit button in the Widget Blueprint.
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Btn_QuitGame;

	UFUNCTION()
	void OnEnterGameClicked();

	UFUNCTION()
	void OnQuitGameClicked();
};
