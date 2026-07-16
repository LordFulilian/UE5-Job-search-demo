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
class UAnimMontage;

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

    UFUNCTION(BlueprintCallable, Category = "UI")
    void ToggleInventoryPanelAction();

    // Active character panel instance exposed for UI coordination.
    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UUserWidget* CharacterPanelInstance;

protected:
    virtual void BeginPlay() override;
    virtual void OnRep_PlayerState() override;
    virtual void OnRep_Controller() override;
    virtual void InitAbilityActorInfo() override;
    virtual int32 GetPlayerLevel() override;

    UPROPERTY(VisibleAnywhere)
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* FollowCamera;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LookAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* ZoomAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* SprintAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* DashAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* JumpAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* OpenPanelAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* LockAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* InteractAction;
    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* OpenInventoryAction;

    UPROPERTY(EditDefaultsOnly, Category = "UI|Inventory")
    TSubclassOf<UInventoryPanelWidget> InventoryPanelClass;

    // Cached inventory panel instance reused across open and close operations.
    UPROPERTY()
    TObjectPtr<UInventoryPanelWidget> InventoryPanelInstance;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    TObjectPtr<UPlayerInputConfig> InputConfig;

    // Native input callbacks.
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
    void Zoom(const FInputActionValue& Value);
    void Dash();
    void SprintStart();
    void SprintStop();
    void Interact();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    float WalkSpeed = 400.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    float SprintSpeed = 1000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    float DashSpeed = 1400.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement|Animation")
    TObjectPtr<UAnimMontage> DashMontage;

    void AbilityInputTagPressed(FGameplayTag InputTag);
    void AbilityInputTagReleased(FGameplayTag InputTag);
    void AbilityInputTagHeld(FGameplayTag InputTag);

    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> CharacterPanelClass;

    // Lock-on targeting.
    AActor* FindBestTarget();
    void ToggleLockOn();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    AActor* LockedTarget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    bool bIsHardLocked = false;

    UPROPERTY(EditDefaultsOnly, Category = "Combat")
    float LockTargetRadius = 1500.f; // Target scan radius.

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
    float InteractionDistance = 350.f;
};
