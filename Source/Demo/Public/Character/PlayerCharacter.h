#pragma once

#include "CoreMinimal.h"
// 请确保这里继承了你正确的父类，比如 ACharacterBase 或 ACharacter
#include "Character/CharacterBase.h" 
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;

UCLASS()
class DEMO_API APlayerCharacter : public ACharacterBase
{
    GENERATED_BODY()

public:
    APlayerCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void PossessedBy(AController* NewController) override;

    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    // 加上 BlueprintCallable，这样你在 Widget 蓝图里也能直接调这个函数关掉面板
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleOpenPanelAction();

    // 加上 BlueprintReadOnly，允许外部安全地读取当前面板实例
    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UUserWidget* CharacterPanelInstance;
protected:
    virtual void BeginPlay() override;
    virtual void OnRep_PlayerState() override;
    virtual void OnRep_Controller() override;
    virtual void InitAbilityActorInfo() override;
    virtual int32 GetPlayerLevel() override;

    // --- 组件 ---
    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* FollowCamera;

    // --- 输入 Action ---
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* ZoomAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* SprintAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* OpenPanelAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LockAction; // ? 锁定按键

    // --- 输入回调函数 ---
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Zoom(const FInputActionValue& Value);
    void SprintStart();
    void SprintStop();
    
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<class UUserWidget> CharacterPanelClass;
 
 

    // ==========================================
    // ? 锁定系统核心变量与函数
    // ==========================================
    AActor* FindBestTarget();
    void ToggleLockOn();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    AActor* LockedTarget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    bool bIsHardLocked = false;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float LockTargetRadius = 1500.f; // 扫描半径
};