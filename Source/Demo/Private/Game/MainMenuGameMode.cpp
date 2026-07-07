// Rylan


#include "Game/MainMenuGameMode.h"

#include "Player/MainMenuPlayerController.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	// 指定我们刚写的纯 UI 控制器
	PlayerControllerClass = AMainMenuPlayerController::StaticClass();
    
	// 主菜单不需要生成实体角色
	DefaultPawnClass = nullptr; 
}