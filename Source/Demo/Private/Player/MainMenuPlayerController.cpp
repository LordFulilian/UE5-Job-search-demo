// Rylan


#include "Player/MainMenuPlayerController.h"

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 1. 强行显示鼠标
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// 2. 限制输入模式为纯 UI 交互，不让游戏世界捕获按键
	FInputModeUIOnly InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    
	SetInputMode(InputModeData);
}