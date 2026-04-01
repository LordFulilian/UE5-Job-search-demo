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
#include "InputActionValue.h"                 
#include "Player/OPlayerState.h"              
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/HUD/PlayerHUD.h"
#include "Blueprint/UserWidget.h"

// 🔴 必须引入高亮接口 (路径请根据实际情况调整)
#include "Interaction/EnemyInterface.h" 

void APlayerCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();
    InitAbilityActorInfo();
}

APlayerCharacter::APlayerCharacter()
{
    // 设置角色的旋转逻辑 (二游经典设置)
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // 配置移动组件
    GetCharacterMovement()->bOrientRotationToMovement = true; 
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 
    GetCharacterMovement()->JumpZVelocity = 700.f; 
    GetCharacterMovement()->AirControl = 0.35f;    

    // 创建弹簧臂 (SpringArm)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f; 
    CameraBoom->bUsePawnControlRotation = true; 

    // 创建摄像机 (Camera)
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
    check(OPlayerState)
    
    OPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(OPlayerState, this);
    Cast<UPlayerAbilitySystemComponent>(OPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
    
    AbilitySystemComponent = OPlayerState->GetAbilitySystemComponent();
    AttributeSet = OPlayerState->GetAttributeSet();
    
    // InitializeDefaultAttributes(); // 如果你在基类声明了它，取消注释即可
    
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

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
       if (MoveAction)
          EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);

       if (LookAction)
          EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);

       if (ZoomAction)
          EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Zoom);

       if (SprintAction)
       {
          EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::SprintStart);
          EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SprintStop);
       }

       if (JumpAction)
       {
          EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
          EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
       }
       
       if (OpenPanelAction)
       {
          EnhancedInputComponent->BindAction(OpenPanelAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleOpenPanelAction);
       }
       
       // 🔴 绑定锁定按键
       if (LockAction)
       {
          EnhancedInputComponent->BindAction(LockAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleLockOn);
       }  
    }
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
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
    // 🔴 如果硬锁定中，禁止鼠标晃动镜头
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
    if (GetCharacterMovement())
    {
       GetCharacterMovement()->MaxWalkSpeed = 1000.f; 
    }
}

void APlayerCharacter::SprintStop()
{
    if (GetCharacterMovement())
    {
       GetCharacterMovement()->MaxWalkSpeed = 400.f; 
    }
}

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
} // 🔴 修复点：正确闭合函数

// ==========================================
// 🔴 锁定系统实现
// ==========================================

AActor* APlayerCharacter::FindBestTarget()
{
    TArray<AActor*> OverlappingActors;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);

    // 球体检测，寻找前方敌人
    UKismetSystemLibrary::SphereOverlapActors(
        this, 
        GetActorLocation(), 
        LockTargetRadius, 
        TArray<TEnumAsByte<EObjectTypeQuery>>{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) },
        AActor::StaticClass(), 
        ActorsToIgnore, 
        OverlappingActors
    );

    AActor* BestTarget = nullptr;
    float HighestScore = -1.f; 

    FVector CameraLocation = FollowCamera->GetComponentLocation();
    FVector CameraForward = FollowCamera->GetForwardVector();

    for (AActor* Actor : OverlappingActors)
    {
        // 确保找到了实现了接口的敌人
        if (!Actor->Implements<UEnemyInterface>()) continue;

        FVector DirToTarget = (Actor->GetActorLocation() - CameraLocation).GetSafeNormal();
        float DotProduct = FVector::DotProduct(CameraForward, DirToTarget);

        if (DotProduct > 0.6f) 
        {
            if (DotProduct > HighestScore)
            {
                HighestScore = DotProduct;
                BestTarget = Actor;
            }
        }
    }

    return BestTarget;
}

void APlayerCharacter::ToggleLockOn()
{
    if (bIsHardLocked && LockedTarget)
    {
        // === 取消锁定 ===
        bIsHardLocked = false;
    
        // 传入 false 关闭高亮
        if (LockedTarget->Implements<UEnemyInterface>())
        {
            IEnemyInterface::Execute_ToggleHighlight(LockedTarget, false);
        }
    
        LockedTarget = nullptr;

        // 恢复自由视角移动模式
        GetCharacterMovement()->bOrientRotationToMovement = true; 
        bUseControllerRotationYaw = false; 
    }
    else
    {
        // === 尝试锁定 ===
        AActor* TargetToLock = FindBestTarget();
        if (TargetToLock)
        {
            LockedTarget = TargetToLock;
            bIsHardLocked = true;

            // 传入 true 开启高亮
            if (LockedTarget->Implements<UEnemyInterface>())
            {
                IEnemyInterface::Execute_ToggleHighlight(LockedTarget, true);
            }

            // 切换战斗环绕移动模式
            GetCharacterMovement()->bOrientRotationToMovement = false; 
            bUseControllerRotationYaw = true; 
        }
    }
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 如果在锁定状态，强制接管摄像机旋转看向敌人
    if (bIsHardLocked)
    {
        if (IsValid(LockedTarget))
        {
            FVector TargetLocation = LockedTarget->GetActorLocation();
            TargetLocation.Z -= 50.f; // 下移防止看天

            FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);
            FRotator CurrentRotation = Controller->GetControlRotation();

            // 平滑插值转动镜头
            FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, LookAtRotation, DeltaTime, 8.0f);
            SmoothedRotation.Roll = 0.f;

            Controller->SetControlRotation(SmoothedRotation);
        }
        else
        {
            // 目标失效(例如死亡被销毁)，解除锁定
            ToggleLockOn();
        }
    }
}