
#include "UI/Widget/PlayerUserWidget.h"
#include "Components/Button.h"
#include "Character/PlayerCharacter.h" 

void UPlayerUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	// 触发蓝图可重写的事件
	WidgetControllerSet(); 
}

// 1. 使用 NativeOnInitialized 替代 NativeConstruct
void UPlayerUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// 整个生命周期只绑定一次，彻底解决第二次打开没反应的问题
	if (close)
	{
		close->OnClicked.AddDynamic(this, &UPlayerUserWidget::OnCloseButtonClicked);
	}
}

void UPlayerUserWidget::NativeDestruct()
{
	if (SubPanelInstance && SubPanelInstance->IsInViewport())
	{
		SubPanelInstance->RemoveFromParent();
		SubPanelInstance = nullptr; 
	}
	Super::NativeDestruct();
}

// 2. 升级关闭逻辑，区分主面板和二级面板
void UPlayerUserWidget::OnCloseButtonClicked()
{
	if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		// 【核心判断】：判断当前的 UI 实例 (this) 是不是角色身上存的那个主面板
		if (PlayerChar->CharacterPanelInstance == this)
		{
			// 如果我是主面板，我就走一套完整的关闭+恢复游戏流程
			PlayerChar->ToggleOpenPanelAction();
		}
		else
		{
			// 如果我不是主面板（说明我是个二级页面），我只负责把自己关掉
			this->RemoveFromParent();
		}
	}
}