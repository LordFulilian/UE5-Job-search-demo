// Rylan


#include "UI/Menu/MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_EnterGame)
	{
		Btn_EnterGame->OnClicked.AddDynamic(this, &UMainMenuWidget::OnEnterGameClicked);
	}

	if (Btn_QuitGame)
	{
		Btn_QuitGame->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitGameClicked);
	}
}

void UMainMenuWidget::OnEnterGameClicked()
{
	if (!GameplayLevel.IsNull())
	{
		if (!GameplayLevel.IsNull())
		{
			// 点击后按钮失效，防止玩家疯狂连点崩溃
			if (Btn_EnterGame) Btn_EnterGame->SetIsEnabled(false);

			// 🔴 使用官方提供的高级 API 切换关卡
			UGameplayStatics::OpenLevelBySoftObjectPtr(this, GameplayLevel);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("你没有在蓝图的 GameplayLevel 变量里选择目标地图！"));
		}
	}
}

void UMainMenuWidget::OnQuitGameClicked()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
}