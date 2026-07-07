#include "Character/PlayerCharacter.h"

// 【核心头文件】
#include "GameFramework/SpringArmComponent.h" 
#include "Camera/CameraComponent.h"           
#include "AbilitySystem/PlayerAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h" 
#include "GameFramework/Controller.h"         
#include "EnhancedInputComponent.h"           
#include "AbilitySystemComponent.h"
#include "Player/OnePlayerController.h"
#include "AbilitySystem/PlayerAbilitySystemLibrary.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "InputActionValue.h"                 
#include "Player/OPlayerState.h"              
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/HUD/PlayerHUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/ItemPickup.h"
#include "Components/InventoryComponent.h" 
#include "UI/Inventory/InventoryPanelWidget.h"
#include "input/PlayerInputComponent.h" 
#include "interaction/EnemyInterface.h" 

void APlayerCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    InitAbilityActorInfo();
}

APlayerCharacter::APlayerCharacter()
{
    // 设置角色的旋转逻辑 (二游经典设置：身体不随相机转，随移动方向转)
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true; 
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
    GetCharacterMovement()->JumpZVelocity = 700.f; 
    GetCharacterMovement()->AirControl = 0.35f;    

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f; 
    CameraBoom->bUsePawnControlRotation = true; 

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
    FollowCamera->bUsePawnControlRotation = false; 
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    InitAbilityActorInfo();
}

void APlayerCharacter::OnRep_Controller()
{
    Super::OnRep_Controller();
    InitAbilityActorInfo();
}

UAbilitySystemComponent* APlayerCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

int32 APlayerCharacter::GetPlayerLevel()
{
    const AOPlayerState* OPlayerState = GetPlayerState<AOPlayerState>();
    check(OPlayerState)
    return OPlayerState->GetPlayerLevel();
}

void APlayerCharacter::InitAbilityActorInfo()
{
    AOPlayerState* OPlayerState = GetPlayerState<AOPlayerState>();
    if (!OPlayerState)
    {
        return;
    }
    
    OPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(OPlayerState, this);
    Cast<UPlayerAbilitySystemComponent>(OPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
    
    AbilitySystemComponent = OPlayerState->GetAbilitySystemComponent();
    AttributeSet = OPlayerState->GetAttributeSet();
    
    // ==========================================
    // 初始化属性 (让血条和攻击力变成非 0)
    // ==========================================
    InitialzeDefaultAttributes();
    
    // Also apply VitalAttributes from CharacterClassInfo if available
    if (UCharacterClassInfo* ClassInfo = UPlayerAbilitySystemLibrary::GetCharacterClassInfo(this))
    {
        if (ClassInfo->VitalAttributes)
        {
            FGameplayEffectContextHandle VitalCtx = AbilitySystemComponent->MakeEffectContext();
            VitalCtx.AddSourceObject(this);
            FGameplayEffectSpecHandle VitalSpec = AbilitySystemComponent->MakeOutgoingSpec(
                ClassInfo->VitalAttributes, 
                static_cast<float>(GetPlayerLevel()), 
                VitalCtx);
            AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*VitalSpec.Data.Get());
            UE_LOG(LogTemp, Log, TEXT("[PlayerCharacter] Applied VitalAttributes from CharacterClassInfo"));
        }
    } 
    
    // ==========================================
    // 赋予初始技能 (让左键平A生效)
    // ==========================================
    if (UPlayerAbilitySystemComponent* PlayerASC = Cast<UPlayerAbilitySystemComponent>(AbilitySystemComponent))
    {
        PlayerASC->AddCharacterAbilities(StartupAbilities); 
    }

    if (AOnePlayerController* OnePlayerController = Cast<AOnePlayerController>(GetController()))
    {
        if (APlayerHUD* PlayerHUD = Cast<APlayerHUD>(OnePlayerController->GetHUD()))
        {
            PlayerHUD->InitOverlay(OnePlayerController, OPlayerState, AbilitySystemComponent, AttributeSet);
        }
    }
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
}

// ==========================================
// 输入绑定核心逻辑
// ==========================================
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    UPlayerInputComponent* AuraInputComponent = CastChecked<UPlayerInputComponent>(PlayerInputComponent);

    // 绑定 GAS 技能动作
    if (InputConfig)
    {
        AuraInputComponent->BindAbilityAction(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
    }

    // 绑定基础动作
    if (MoveAction)
        AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

    if (LookAction)
        AuraInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

    if (ZoomAction)
        AuraInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Zoom);

    if (SprintAction)
    {
        AuraInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SprintStart);
        AuraInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SprintStop);
    }

    if (JumpAction)
    {
        AuraInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        AuraInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
    }
    
    // 绑定交互/拾取
    if (InteractAction)
    {
        AuraInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);
    }

    // 绑定锁定
    if (LockAction)
        AuraInputComponent->BindAction(LockAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleLockOn);

    // 🔴 角色面板开启键
    if (OpenPanelAction)
        AuraInputComponent->BindAction(OpenPanelAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleOpenPanelAction);
          
    // 🔴 背包面板开启键
    if (OpenInventoryAction)
        AuraInputComponent->BindAction(OpenInventoryAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleInventoryPanelAction);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
    if (GetAbilitySystemComponent() && GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Attacking"))))
    {
        return;
    }
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
       const FRotator Rotation = Controller->GetControlRotation();
       const FRotator YawRotation(0, Rotation.Yaw, 0); 

       const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
       const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

       AddMovementInput(ForwardDirection, MovementVector.Y);
       AddMovementInput(RightDirection, MovementVector.X);
    }
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
    if (bIsHardLocked) return;
    
    FVector2D LookAxisVector = Value.Get<FVector2D>();
    if (Controller != nullptr)
    {
       AddControllerYawInput(LookAxisVector.X);
       AddControllerPitchInput(LookAxisVector.Y);
    }
}

void APlayerCharacter::Zoom(const FInputActionValue& Value)
{
    float ZoomValue = Value.Get<float>();
    if (CameraBoom)
    {
       const float MinDistance = 150.0f; 
       const float MaxDistance = 1000.0f; 
       const float ZoomSpeed = 20.0f;     

       float NewDistance = CameraBoom->TargetArmLength - (ZoomValue * ZoomSpeed);
       CameraBoom->TargetArmLength = FMath::Clamp(NewDistance, MinDistance, MaxDistance);
    }
}

void APlayerCharacter::SprintStart()
{
    if (GetCharacterMovement()) GetCharacterMovement()->MaxWalkSpeed = 1000.f; 
}

void APlayerCharacter::SprintStop()
{
    if (GetCharacterMovement()) GetCharacterMovement()->MaxWalkSpeed = 400.f; 
}

void APlayerCharacter::Interact()
{
    UInventoryComponent* Inventory = FindComponentByClass<UInventoryComponent>();
    if (!Inventory) return;
    
    TArray<AActor*> OverlappingActors;
    GetOverlappingActors(OverlappingActors, AItemPickup::StaticClass());
    
    for (AActor* OverlappedActor : OverlappingActors)
    {
        if (AItemPickup* Pickup = Cast<AItemPickup>(OverlappedActor))
        {
            if (Pickup->ItemID == NAME_None)
            {
                continue;
            }

            // 装入背包
            if (Inventory->AddItem(Pickup->ItemID, 1))
            {
                Pickup->Destroy();
            }
        }
    }
}

void APlayerCharacter::AbilityInputTagPressed(FGameplayTag InputTag)
{
    if (UPlayerAbilitySystemComponent* PlayerASC = Cast<UPlayerAbilitySystemComponent>(GetAbilitySystemComponent()))
    {
        PlayerASC->AbilityInputTagPressed(InputTag);
    }
}

void APlayerCharacter::AbilityInputTagReleased(FGameplayTag InputTag)
{
    if (UPlayerAbilitySystemComponent* PlayerASC = Cast<UPlayerAbilitySystemComponent>(GetAbilitySystemComponent()))
    {
        PlayerASC->AbilityInputTagReleased(InputTag);
    }
}

void APlayerCharacter::AbilityInputTagHeld(FGameplayTag InputTag)
{
    if (UPlayerAbilitySystemComponent* PlayerASC = Cast<UPlayerAbilitySystemComponent>(GetAbilitySystemComponent()))
    {
        PlayerASC->AbilityInputTagHeld(InputTag); 
    }
}

// ========================================================
// 🔴 角色属性面板 
// ========================================================
void APlayerCharacter::ToggleOpenPanelAction()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    if (CharacterPanelInstance && CharacterPanelInstance->IsInViewport())
    {
       CharacterPanelInstance->RemoveFromParent();
       FInputModeGameOnly InputMode;
       PC->SetInputMode(InputMode);
       PC->bShowMouseCursor = false;
       PC->SetPause(false); 
    }
    else
    {
       if (CharacterPanelInstance == nullptr && CharacterPanelClass != nullptr)
       {
          CharacterPanelInstance = CreateWidget<UUserWidget>(PC, CharacterPanelClass);
       }
       if (CharacterPanelInstance)
       {
          CharacterPanelInstance->AddToViewport();
          FInputModeGameAndUI InputMode;
          InputMode.SetWidgetToFocus(CharacterPanelInstance->TakeWidget()); 
          InputMode.SetHideCursorDuringCapture(false);
          PC->SetInputMode(InputMode);
          PC->bShowMouseCursor = true;
          PC->SetPause(true); 
       }
    }
}

// ========================================================
// 🔴 背包面板 
// ========================================================
void APlayerCharacter::ToggleInventoryPanelAction()
{
    APlayerController* PC = Cast<APlayerController>(GetController());
    if (!PC) return;

    if (InventoryPanelInstance && InventoryPanelInstance->IsInViewport())
    {
        InventoryPanelInstance->RemoveFromParent();
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
        PC->SetPause(false); 
    }
    else
    {
        if (InventoryPanelInstance == nullptr && InventoryPanelClass != nullptr)
        {
            InventoryPanelInstance = CreateWidget<UInventoryPanelWidget>(PC, InventoryPanelClass);
        }
   
        if (InventoryPanelInstance)
        {
            // 核心注射环节：拿到玩家身上的背包，喂给 UI！
            UInventoryComponent* InventoryComp = FindComponentByClass<UInventoryComponent>();
            if (InventoryComp)
            {
                InventoryPanelInstance->InitializePanel(InventoryComp);
            }

            InventoryPanelInstance->AddToViewport();
            FInputModeGameAndUI InputMode;
            InputMode.SetWidgetToFocus(InventoryPanelInstance->TakeWidget()); 
            InputMode.SetHideCursorDuringCapture(false);
            PC->SetInputMode(InputMode);
            PC->bShowMouseCursor = true;
            PC->SetPause(true); 
        }
    }
}

// --- 锁定系统 ---

AActor* APlayerCharacter::FindBestTarget()
{
    TArray<AActor*> OverlappingActors;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);

    UKismetSystemLibrary::SphereOverlapActors(
        this, GetActorLocation(), LockTargetRadius, 
        TArray<TEnumAsByte<EObjectTypeQuery>>{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) },
        AActor::StaticClass(), ActorsToIgnore, OverlappingActors
    );

    AActor* BestTarget = nullptr;
    float HighestScore = -1.f; 
    FVector CameraLocation = FollowCamera->GetComponentLocation();
    FVector CameraForward = FollowCamera->GetForwardVector();

    for (AActor* Actor : OverlappingActors)
    {
        if (!Actor->Implements<UEnemyInterface>()) continue;
        FVector DirToTarget = (Actor->GetActorLocation() - CameraLocation).GetSafeNormal();
        float DotProduct = FVector::DotProduct(CameraForward, DirToTarget);

        if (DotProduct > 0.6f && DotProduct > HighestScore)
        {
            HighestScore = DotProduct;
            BestTarget = Actor;
        }
    }
    return BestTarget;
}

void APlayerCharacter::ToggleLockOn()
{
    if (bIsHardLocked && LockedTarget)
    {
        bIsHardLocked = false;
        if (LockedTarget->Implements<UEnemyInterface>()) IEnemyInterface::Execute_ToggleHighlight(LockedTarget, false);
        LockedTarget = nullptr;
        GetCharacterMovement()->bOrientRotationToMovement = true; 
        bUseControllerRotationYaw = false; 
    }
    else
    {
        AActor* TargetToLock = FindBestTarget();
        if (TargetToLock)
        {
            LockedTarget = TargetToLock;
            bIsHardLocked = true;
            if (LockedTarget->Implements<UEnemyInterface>()) IEnemyInterface::Execute_ToggleHighlight(LockedTarget, true);
            GetCharacterMovement()->bOrientRotationToMovement = false; 
            bUseControllerRotationYaw = true; 
        }
    }
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsHardLocked && IsValid(LockedTarget))
    {
        FVector TargetLocation = LockedTarget->GetActorLocation();
        TargetLocation.Z -= 50.f; 
        FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
        FRotator CurrentRotation = Controller->GetControlRotation();
        FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, 8.0f);
        SmoothedRotation.Roll = 0.f;
        Controller->SetControlRotation(SmoothedRotation);
    }
    else if (bIsHardLocked)
    {
        ToggleLockOn();
    }
}