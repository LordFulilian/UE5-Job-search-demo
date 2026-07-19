// OnePlayerController.cpp

#include "Player/OnePlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameModeBase.h"
#include "UI/Widget/DamageTextComponent.h"
#include "AbilitySystem/PlayerAttributeSet.h"
#include "AbilitySystem/Data/HeroUIInfo.h"
#include "Character/PlayerCharacter.h"
#include "Components/PartyComponent.h"
#include "Components/QuestComponent.h"
#include "Dialogue/DialogueDataAsset.h"
#include "interaction/InteractableInterface.h"
#include "UI/Widget/PlayerUserWidget.h"
#include "Character/BossCharacter.h"
#include "UI/Dialogue/DialogueWidget.h"
#include "Player/OPlayerState.h"
#include "InputCoreTypes.h"
#include "Components/SkeletalMeshComponent.h"

AOnePlayerController::AOnePlayerController()
{
    bReplicates = true;
    DialogueWidgetClass = UDialogueWidget::StaticClass();
}
void AOnePlayerController::ShowBossHealthBar(ABossCharacter* Boss)
{
    if (!IsLocalController() || !Boss || !BossHealthBarClass) return;

    ActiveBoss = Boss;

    if (!BossHealthBarInstance)
    {
        BossHealthBarInstance =
            CreateWidget<UPlayerUserWidget>(this, BossHealthBarClass);
    }

    if (BossHealthBarInstance)
    {
        if (!BossHealthBarInstance->IsInViewport())
        {
            BossHealthBarInstance->AddToViewport(50);
        }
        BossHealthBarInstance->SetWidgetController(Boss);

        if (const UPlayerAttributeSet* BossAttributes =
            Cast<UPlayerAttributeSet>(Boss->GetAttributeSet()))
        {
            Boss->OnHealthChanged.Broadcast(BossAttributes->GetHealth());
            Boss->OnMaxHealthChanged.Broadcast(BossAttributes->GetMaxHealth());
        }
    }
}

void AOnePlayerController::HideBossHealthBar()
{
    if (BossHealthBarInstance)
    {
        BossHealthBarInstance->RemoveFromParent();
    }
}

void AOnePlayerController::LeaveBossEncounter(ABossCharacter* Boss)
{
    if (!IsLocalController() || !Boss || ActiveBoss.Get() != Boss) return;

    ActiveBoss.Reset();
    HideBossHealthBar();
}

void AOnePlayerController::ClientShowDeathScreen_Implementation()
{
    if (!IsLocalController()) return;

    HideBossHealthBar();
    if (!DeathScreenClass)
    {
        UE_LOG(LogTemp, Error,
            TEXT("%s requires DeathScreenClass to display the respawn UI."),
            *GetName());
        return;
    }

    if (!DeathScreenInstance)
    {
        DeathScreenInstance = CreateWidget<UUserWidget>(this, DeathScreenClass);
    }
    if (!DeathScreenInstance) return;

    if (!DeathScreenInstance->IsInViewport())
    {
        DeathScreenInstance->AddToViewport(200);
    }

    FInputModeUIOnly InputMode;
    SetInputMode(InputMode);
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    bShowMouseCursor = true;
}

void AOnePlayerController::RequestRespawn()
{
    if (HasAuthority())
    {
        HandleRespawn();
    }
    else
    {
        ServerRequestRespawn();
    }
}

void AOnePlayerController::ServerRequestRespawn_Implementation()
{
    HandleRespawn();
}

void AOnePlayerController::HandleRespawn()
{
    APlayerCharacter* DeadCharacter = Cast<APlayerCharacter>(GetPawn());
    if (!DeadCharacter || !DeadCharacter->IsDeathSequenceStarted()) return;

    UAbilitySystemComponent* PlayerASC = DeadCharacter->GetAbilitySystemComponent();
    if (PlayerASC)
    {
        PlayerASC->CancelAllAbilities();
    }

    APawn* PawnToDestroy = GetPawn();
    UnPossess();
    if (IsValid(PawnToDestroy))
    {
        PawnToDestroy->Destroy();
    }

    if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AGameModeBase>())
    {
        GameMode->RestartPlayer(this);
    }

    if (PlayerASC)
    {
        const float MaxHealth = PlayerASC->GetNumericAttribute(
            UPlayerAttributeSet::GetMaxHealthAttribute());
        PlayerASC->SetNumericAttributeBase(
            UPlayerAttributeSet::GetHealthAttribute(), MaxHealth);
    }

    ClientHideDeathScreen();
}

void AOnePlayerController::ClientHideDeathScreen_Implementation()
{
    if (DeathScreenInstance)
    {
        DeathScreenInstance->RemoveFromParent();
    }

    SetIgnoreMoveInput(false);
    SetIgnoreLookInput(false);
    bShowMouseCursor = false;
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);

    if (ABossCharacter* Boss = ActiveBoss.Get())
    {
        if (Boss->BossPhase != EBossPhase::Dormant &&
            Boss->BossPhase != EBossPhase::Dead)
        {
            ShowBossHealthBar(Boss);
        }
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
        if (AltAction)
        {
            EnhancedInputComponent->BindAction(AltAction, ETriggerEvent::Started, this, &AOnePlayerController::ToggleMouseCursor);
        }

        if (ClickAction)
        {
            EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Started, this, &AOnePlayerController::OnClickScreen);
        }

        if (QuestLogAction)
        {
            EnhancedInputComponent->BindAction(
                QuestLogAction, ETriggerEvent::Started,
                this, &AOnePlayerController::ToggleQuestList);
        }

        if (PartyAction)
        {
            EnhancedInputComponent->BindAction(
                PartyAction, ETriggerEvent::Started,
                this, &AOnePlayerController::TogglePartyPage);
        }
        else
        {
            // Keep the party screen usable when a controller Blueprint has not
            // assigned IA_Party yet. IMC_PlayerContext maps IA_Party to L.
            InputComponent->BindKey(EKeys::L, IE_Pressed,
                this, &AOnePlayerController::TogglePartyPage);
        }
    }

    // Party switching remains available even when the Enhanced Input mapping
    // context has not yet been updated with four dedicated actions.
    InputComponent->BindKey(EKeys::One, IE_Pressed,
        this, &AOnePlayerController::SwitchToPartySlot1);
    InputComponent->BindKey(EKeys::Two, IE_Pressed,
        this, &AOnePlayerController::SwitchToPartySlot2);
    InputComponent->BindKey(EKeys::Three, IE_Pressed,
        this, &AOnePlayerController::SwitchToPartySlot3);
    InputComponent->BindKey(EKeys::Four, IE_Pressed,
        this, &AOnePlayerController::SwitchToPartySlot4);
}

void AOnePlayerController::SwitchToPartySlot1() { SwitchToPartySlot(0); }
void AOnePlayerController::SwitchToPartySlot2() { SwitchToPartySlot(1); }
void AOnePlayerController::SwitchToPartySlot3() { SwitchToPartySlot(2); }
void AOnePlayerController::SwitchToPartySlot4() { SwitchToPartySlot(3); }

void AOnePlayerController::SwitchToPartySlot(int32 SlotIndex)
{
    if (!IsLocalController() || IsPaused() || ActiveDialogueWidget ||
        (PartyWidgetInstance && PartyWidgetInstance->IsInViewport()) ||
        (QuestListWidgetInstance && QuestListWidgetInstance->IsInViewport()))
    {
        return;
    }

    if (HasAuthority())
    {
        PerformPartySwitch(SlotIndex);
    }
    else
    {
        ServerSwitchToPartySlot(SlotIndex);
    }
}

void AOnePlayerController::ServerSwitchToPartySlot_Implementation(int32 SlotIndex)
{
    PerformPartySwitch(SlotIndex);
}

void AOnePlayerController::PerformPartySwitch(int32 SlotIndex)
{
    AOPlayerState* OPlayerState = GetPlayerState<AOPlayerState>();
    UPartyComponent* PartyComponent = OPlayerState
        ? OPlayerState->GetPartyComponent()
        : nullptr;
    if (!PartyComponent) return;

    const TArray<FPartySlotViewData> PartySlots = PartyComponent->GetPartySlots();
    if (!PartySlots.IsValidIndex(SlotIndex) ||
        !PartySlots[SlotIndex].bOccupied)
    {
        return;
    }

    const TSubclassOf<APlayerCharacter> CharacterClass =
        PartySlots[SlotIndex].HeroInfo.CharacterClass.LoadSynchronous();
    APlayerCharacter* CurrentCharacter = Cast<APlayerCharacter>(GetPawn());
    if (!CharacterClass || (CurrentCharacter &&
        CurrentCharacter->IsA(CharacterClass)))
    {
        return;
    }

    const FTransform SpawnTransform = CurrentCharacter
        ? CurrentCharacter->GetActorTransform()
        : FTransform::Identity;
    FActorSpawnParameters SpawnParameters;
    SpawnParameters.Owner = this;
    SpawnParameters.Instigator = CurrentCharacter;
    SpawnParameters.SpawnCollisionHandlingOverride =
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    APlayerCharacter* NewCharacter = GetWorld()->SpawnActor<APlayerCharacter>(
        CharacterClass, SpawnTransform, SpawnParameters);
    if (!NewCharacter) return;

    APawn* PreviousPawn = GetPawn();
    Possess(NewCharacter);

    if (USkeletalMeshComponent* CharacterMesh = NewCharacter->GetMesh())
    {
        CharacterMesh->InitAnim(true);
    }
    if (IsValid(PreviousPawn)) PreviousPawn->Destroy();
}

void AOnePlayerController::ToggleQuestList()
{
    if (!IsLocalController()) return;

    if (QuestListWidgetInstance && QuestListWidgetInstance->IsInViewport())
    {
        CloseManagedMenu(QuestListWidgetInstance);
        return;
    }

    AOPlayerState* OPlayerState = GetPlayerState<AOPlayerState>();
    if (!OPlayerState || !OPlayerState->GetQuestComponent()) return;
    if (!QuestListWidgetClass)
    {
        UE_LOG(LogTemp, Error,
            TEXT("%s requires QuestListWidgetClass."), *GetName());
        return;
    }

    if (PartyWidgetInstance && PartyWidgetInstance->IsInViewport())
    {
        PartyWidgetInstance->RemoveFromParent();
    }
    if (!QuestListWidgetInstance)
    {
        QuestListWidgetInstance =
            CreateWidget<UPlayerUserWidget>(this, QuestListWidgetClass);
    }
    OpenManagedMenu(QuestListWidgetInstance, OPlayerState->GetQuestComponent());
}

void AOnePlayerController::TogglePartyPage()
{
    if (!IsLocalController()) return;

    if (PartyWidgetInstance && PartyWidgetInstance->IsInViewport())
    {
        CloseManagedMenu(PartyWidgetInstance);
        return;
    }

    AOPlayerState* OPlayerState = GetPlayerState<AOPlayerState>();
    if (!OPlayerState || !OPlayerState->GetPartyComponent()) return;
    if (!PartyWidgetClass)
    {
        UE_LOG(LogTemp, Error,
            TEXT("%s requires PartyWidgetClass."), *GetName());
        return;
    }

    if (QuestListWidgetInstance && QuestListWidgetInstance->IsInViewport())
    {
        QuestListWidgetInstance->RemoveFromParent();
    }
    if (!PartyWidgetInstance)
    {
        PartyWidgetInstance =
            CreateWidget<UPlayerUserWidget>(this, PartyWidgetClass);
    }
    OpenManagedMenu(PartyWidgetInstance, OPlayerState->GetPartyComponent());
}

void AOnePlayerController::OpenManagedMenu(
    UPlayerUserWidget* Widget, UObject* DataSource)
{
    if (!Widget || !DataSource) return;

    Widget->SetWidgetController(DataSource);
    if (!Widget->IsInViewport()) Widget->AddToViewport(100);

    FInputModeGameAndUI InputMode;
    InputMode.SetWidgetToFocus(Widget->TakeWidget());
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);
    bShowMouseCursor = true;
    SetIgnoreMoveInput(true);
    SetIgnoreLookInput(true);
    SetPause(true);
}

bool AOnePlayerController::CloseManagedMenu(UPlayerUserWidget* Widget)
{
    if (!Widget ||
        (Widget != QuestListWidgetInstance && Widget != PartyWidgetInstance))
    {
        return false;
    }

    Widget->RemoveFromParent();
    RestoreGameplayInput();
    return true;
}

void AOnePlayerController::RestoreGameplayInput()
{
    const bool bAnotherMenuOpen =
        (QuestListWidgetInstance && QuestListWidgetInstance->IsInViewport()) ||
        (PartyWidgetInstance && PartyWidgetInstance->IsInViewport());
    if (bAnotherMenuOpen) return;

    SetPause(false);
    SetIgnoreMoveInput(false);
    SetIgnoreLookInput(false);
    bShowMouseCursor = false;
    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
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
