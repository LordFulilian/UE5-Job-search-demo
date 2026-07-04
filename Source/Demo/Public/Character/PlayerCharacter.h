#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h" 
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "input/PlayerInputConfig.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInventoryPanelWidget;
class UInventoryComponent;

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

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleOpenPanelAction();

    // ? 这一行声明就是解开报错的关键！
    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleInventoryPanelAction();

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
    UInputAction* LockAction; 
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* InteractAction; // 交互/拾取按键
    
    // ? 背包输入动作
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* OpenInventoryAction;
    
    UPROPERTY(EditDefaultsOnly, Category = "UI|Inventory")
    TSubclassOf<UInventoryPanelWidget> InventoryPanelClass;

    // 3. 用来缓存生成的面板实例
    UPROPERTY()
    TObjectPtr<UInventoryPanelWidget> InventoryPanelInstance;
  

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UPlayerInputConfig> InputConfig;
    
    // --- 输入回调函数 ---
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Zoom(const FInputActionValue& Value);
    void SprintStart();
    void SprintStop();
    void Interact();
    
    void AbilityInputTagPressed(FGameplayTag InputTag);
    void AbilityInputTagReleased(FGameplayTag InputTag);
    void AbilityInputTagHeld(FGameplayTag InputTag);
    
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf< UUserWidget> CharacterPanelClass;
 

    // ==========================================
    // 锁定系统核心变量与函数
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