#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "interaction/InteractableInterface.h"
#include "NPCCharacter.generated.h"

class UInteractionPromptWidget;
class UDialogueDataAsset;
class UQuestDataAsset;
class UQuestComponent;
class USphereComponent;
class UUserWidget;
class UWidgetComponent;

UCLASS()
class DEMO_API ANPCCharacter : public ACharacter, public IInteractableInterface
{
	GENERATED_BODY()

public:
	ANPCCharacter();
	virtual bool CanInteract_Implementation(APlayerCharacter* Player) const override;
	virtual void Interact_Implementation(APlayerCharacter* Player) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual void OnInteractionFinished_Implementation(APlayerCharacter* Player) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void HandleInteractionBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void HandleInteractionEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<USphereComponent> InteractionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UWidgetComponent> InteractionPromptComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UWidgetComponent> QuestMarkerComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction")
	TSubclassOf<UInteractionPromptWidget> InteractionPromptWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<UDialogueDataAsset> DialogueData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText InteractionPrompt = NSLOCTEXT("Interaction", "TalkPrompt", "Talk");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UQuestDataAsset> QuestToOffer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Quest")
	TSubclassOf<UUserWidget> QuestMarkerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	bool bRepeatable = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Dialogue")
	bool bDialogueCompleted = false;

private:
	void BindQuestEvents(APlayerCharacter* Player);

	UFUNCTION()
	void RefreshQuestMarker();

	void SetInteractionPromptVisible(
		bool bVisible, APlayerCharacter* Player = nullptr);

	UPROPERTY(Transient)
	TObjectPtr<UDialogueDataAsset> RuntimeDialogueData;

	UPROPERTY(Transient)
	TObjectPtr<UQuestComponent> BoundQuestComponent;
};
