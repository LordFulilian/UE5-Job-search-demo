// Rylan


#include "Player/MainMenuPlayerController.h"

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Keep the cursor visible while the menu is active.
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// Route input exclusively to the menu UI.
	FInputModeUIOnly InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    
	SetInputMode(InputModeData);
}
