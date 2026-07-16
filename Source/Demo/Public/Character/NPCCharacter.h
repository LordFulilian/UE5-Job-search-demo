#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "interaction/InteractableInterface.h"
#include "NPCCharacter.generated.h"

class ABossCharacter;
class UDialogueDataAsset;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TObjectPtr<UDialogueDataAsset> DialogueData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText InteractionPrompt = NSLOCTEXT("Interaction", "TalkPrompt", "Talk");

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Boss")
	TObjectPtr<ABossCharacter> BossToActivate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	bool bRepeatable = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Dialogue")
	bool bDialogueCompleted = false;

private:
	UPROPERTY(Transient)
	TObjectPtr<UDialogueDataAsset> RuntimeDialogueData;
};
