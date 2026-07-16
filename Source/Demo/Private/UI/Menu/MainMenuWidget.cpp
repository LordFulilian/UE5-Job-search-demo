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
			// Disable the button to prevent duplicate level-load requests.
			if (Btn_EnterGame) Btn_EnterGame->SetIsEnabled(false);

			// Load the configured gameplay level through the engine API.
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
