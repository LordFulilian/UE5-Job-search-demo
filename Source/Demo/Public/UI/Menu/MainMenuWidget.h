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
	// 在编辑器里配置真正游戏关卡的名字（比如 "Map_Gameplay"）
	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	TSoftObjectPtr<UWorld> GameplayLevel;

protected:
	virtual void NativeConstruct() override;

	// 🔴 自动化绑定蓝图中的“进入游戏”按钮
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_EnterGame;

	// 🔴 退出游戏按钮（通常独立游戏都会规范配上）
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> Btn_QuitGame;

	UFUNCTION()
	void OnEnterGameClicked();

	UFUNCTION()
	void OnQuitGameClicked();
};