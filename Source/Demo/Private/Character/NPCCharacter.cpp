#include "Character/NPCCharacter.h"

#include "Character/BossCharacter.h"
#include "Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Dialogue/DialogueDataAsset.h"
#include "EngineUtils.h"
#include "Player/OnePlayerController.h"

ANPCCharacter::ANPCCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void ANPCCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!DialogueData)
	{
		RuntimeDialogueData = NewObject<UDialogueDataAsset>(this);
		RuntimeDialogueData->Lines = {
			{NSLOCTEXT("BossDialogue", "GuideName1", "Guardian"), NSLOCTEXT("BossDialogue", "GuideLine1", "The creature ahead guards the arena. Once you enter, it will not let you leave."), nullptr},
			{NSLOCTEXT("BossDialogue", "PlayerName", "Player"), NSLOCTEXT("BossDialogue", "PlayerLine", "Then I will defeat it."), nullptr},
			{NSLOCTEXT("BossDialogue", "GuideName2", "Guardian"), NSLOCTEXT("BossDialogue", "GuideLine2", "Be careful. When badly wounded, it enters a second phase."), nullptr}
		};
		DialogueData = RuntimeDialogueData;
	}

	if (!BossToActivate)
	{
		for (TActorIterator<ABossCharacter> It(GetWorld()); It; ++It)
		{
			BossToActivate = *It;
			break;
		}
	}
}

bool ANPCCharacter::CanInteract_Implementation(APlayerCharacter* Player) const
{
	return IsValid(Player) && IsValid(DialogueData) && (bRepeatable || !bDialogueCompleted);
}

void ANPCCharacter::Interact_Implementation(APlayerCharacter* Player)
{
	if (!CanInteract_Implementation(Player)) return;
	if (AOnePlayerController* PC = Cast<AOnePlayerController>(Player->GetController()))
	{
		PC->OpenDialogue(DialogueData, this);
	}
}

FText ANPCCharacter::GetInteractionPrompt_Implementation() const
{
	return InteractionPrompt;
}

void ANPCCharacter::OnInteractionFinished_Implementation(APlayerCharacter* Player)
{
	bDialogueCompleted = true;
	if (BossToActivate) BossToActivate->ActivateBoss();
}
