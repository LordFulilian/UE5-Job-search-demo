// OnePlayerController.cpp

#include "Player/OnePlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"
#include "Character/PlayerCharacter.h"
#include "Dialogue/DialogueDataAsset.h"
#include "interaction/InteractableInterface.h"
#include "UI/Widget/PlayerUserWidget.h"
#include "Character/BossCharacter.h"
#include "UI/Dialogue/DialogueWidget.h"

AOnePlayerController::AOnePlayerController()
{
    bReplicates = true;
    DialogueWidgetClass = UDialogueWidget::StaticClass();
}
void AOnePlayerController::ShowBossHealthBar(ABossCharacter* Boss)
{
    if (!IsLocalController() || !Boss || !BossHealthBarClass) return;

    if (!BossHealthBarInstance)
    {
        BossHealthBarInstance =
            CreateWidget<UPlayerUserWidget>(this, BossHealthBarClass);
    }

    if (BossHealthBarInstance)
    {
        BossHealthBarInstance->SetWidgetController(Boss);
        BossHealthBarInstance->AddToViewport(50);
    }
}

void AOnePlayerController::HideBossHealthBar()
{
    if (BossHealthBarInstance)
    {
        BossHealthBarInstance->RemoveFromParent();
    }
}
void AOnePlayerController::ToggleMouseCursor()
{
    bShowMouseCursor = !bShowMouseCursor;

    // Switch input routing with cursor visibility.
    if (bShowMouseCursor)
    {
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        InputMode.SetHideCursorDuringCapture(false);
        SetInputMode(InputMode);
    }
    else
    {
        FInputModeGameOnly InputMode;
        SetInputMode(InputMode);
    }
}

void AOnePlayerController::OnClickScreen()
{
    // Clicking the viewport returns focus to the game only when the cursor is visible.
    if (bShowMouseCursor)
    {
        ToggleMouseCursor();
    }
}

void AOnePlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(AltAction, ETriggerEvent::Started, this, &AOnePlayerController::ToggleMouseCursor);

        if (ClickAction)
        {
            EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AOnePlayerController::OnClickScreen);
        }
    }
}

void AOnePlayerController::OpenDialogue(UDialogueDataAsset* DialogueData, AActor* InteractionSource)
{
    if (!IsLocalController() || !DialogueData || !DialogueWidgetClass || ActiveDialogueWidget) return;

    ActiveDialogueWidget = CreateWidget<UDialogueWidget>(this, DialogueWidgetClass);
    if (!ActiveDialogueWidget) return;

    ActiveInteractionSource = InteractionSource;
    ActiveDialogueWidget->OnDialogueFinished.AddDynamic(this, &AOnePlayerController::HandleDialogueFinished);
    ActiveDialogueWidget->AddToViewport(100);

    FInputModeGameAndUI InputMode;
    InputMode.SetWidgetToFocus(ActiveDialogueWidget->TakeWidget());
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);
    bShowMouseCursor = true;
    SetPause(true);
    ActiveDialogueWidget->InitializeDialogue(DialogueData);
}

void AOnePlayerController::CloseDialogue()
{
    if (ActiveDialogueWidget)
    {
        ActiveDialogueWidget->RemoveFromParent();
        ActiveDialogueWidget = nullptr;
    }

    ActiveInteractionSource = nullptr;
    SetPause(false);
    bShowMouseCursor = false;
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
}

void AOnePlayerController::HandleDialogueFinished()
{
    AActor* FinishedSource = ActiveInteractionSource;
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
    CloseDialogue();

    if (IsValid(FinishedSource) && FinishedSource->Implements<UInteractableInterface>())
    {
        IInteractableInterface::Execute_OnInteractionFinished(FinishedSource, PlayerCharacter);
    }
}

void AOnePlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bCriticalHit)
{
    if (IsValid(TargetCharacter) && DamageTextComponentClass)
    {
        UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
        DamageText->RegisterComponent();
        DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
        DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        DamageText->DamageText(DamageAmount, bCriticalHit);
    }
}

void AOnePlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = false;

    FInputModeGameOnly InputModeData;
    InputModeData.SetConsumeCaptureMouseDown(true);
    SetInputMode(InputModeData);

    // Install the gameplay mapping context for the local player.
    if (PlayerContext)
    {
        if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
            {
                Subsystem->AddMappingContext(PlayerContext, 0);
            }
        }
    }
}
