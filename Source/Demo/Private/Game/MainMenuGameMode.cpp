// Rylan


#include "Game/MainMenuGameMode.h"

#include "Player/MainMenuPlayerController.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	// Use the menu-only player controller.
	PlayerControllerClass = AMainMenuPlayerController::StaticClass();
    
	// The main menu does not spawn a playable pawn.
	DefaultPawnClass = nullptr; 
}
