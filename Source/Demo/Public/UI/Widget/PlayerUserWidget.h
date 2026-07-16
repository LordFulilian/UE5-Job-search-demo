// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerUserWidget.generated.h"

// Forward declarations keep UI headers lightweight.
class UButton;
class APlayerCharacter;

/**
 * Base player widget with controller injection and shared close behavior.
 */
UCLASS()
class DEMO_API UPlayerUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Injects the widget controller and notifies the Blueprint.
	UFUNCTION(BlueprintCallable, Category = "UI|Controller")
	void SetWidgetController(UObject* InWidgetController);

	// Controller currently driving this widget.
	UPROPERTY(BlueprintReadOnly, Category = "UI|Controller")
	TObjectPtr<UObject> WidgetController;

protected:
	// Implemented by Blueprints that bind controller delegates.
	UFUNCTION(BlueprintImplementableEvent, Category = "UI|Controller")
	void WidgetControllerSet();

	// UI lifecycle.

	// Binds native widget events.
	virtual void NativeOnInitialized() override;

	// Removes any child panel owned by this widget.
	virtual void NativeDestruct() override;

	// Optionally binds a Designer button named "close".
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> close;

	// Handles the optional close button.
	UFUNCTION()
	void OnCloseButtonClicked();

	// Child panel removed when this widget is destroyed.
	UPROPERTY(BlueprintReadWrite, Category = "UI|SubPanel")
	TObjectPtr<UPlayerUserWidget> SubPanelInstance; 
};
