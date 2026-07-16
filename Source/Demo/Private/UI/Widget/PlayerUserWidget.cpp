
#include "UI/Widget/PlayerUserWidget.h"
#include "Components/Button.h"
#include "Character/PlayerCharacter.h" 

void UPlayerUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	// Let the Blueprint bind to its newly assigned controller.
	WidgetControllerSet(); 
}

// Bind native events once for the lifetime of the widget.
void UPlayerUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	// NativeOnInitialized avoids duplicate bindings across repeated opens.
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

// Main panels restore game input; child panels only remove themselves.
void UPlayerUserWidget::OnCloseButtonClicked()
{
	if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetOwningPlayerPawn()))
	{
		// Compare against the main panel cached by the character.
		if (PlayerChar->CharacterPanelInstance == this)
		{
			// The character owns the full close and input-restoration flow.
			PlayerChar->ToggleOpenPanelAction();
		}
		else
		{
			// Secondary panels can remove themselves directly.
			this->RemoveFromParent();
		}
	}
}
