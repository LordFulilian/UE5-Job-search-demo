#include "Character/NPCCharacter.h"

#include "Character/PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/QuestComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Dialogue/DialogueDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Player/OnePlayerController.h"
#include "Quest/QuestDataAsset.h"
#include "interaction/InteractionPromptWidget.h"

ANPCCharacter::ANPCCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	InteractionSphere =
		CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(GetRootComponent());
	InteractionSphere->InitSphereRadius(250.f);
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionSphere->SetCollisionResponseToChannel(
		ECC_GameTraceChannel1, ECR_Overlap);

	InteractionPromptComponent =
		CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionPrompt"));
	InteractionPromptComponent->SetupAttachment(GetRootComponent());
	InteractionPromptComponent->SetRelativeLocation(FVector(0.f, 0.f, 140.f));
	InteractionPromptComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractionPromptComponent->SetDrawAtDesiredSize(true);
	InteractionPromptComponent->SetPivot(FVector2D(0.5f, 1.f));
	InteractionPromptComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractionPromptComponent->SetVisibility(false);

	QuestMarkerComponent =
		CreateDefaultSubobject<UWidgetComponent>(TEXT("QuestMarker"));
	QuestMarkerComponent->SetupAttachment(GetRootComponent());
	QuestMarkerComponent->SetRelativeLocation(FVector(0.f, 0.f, 190.f));
	QuestMarkerComponent->SetWidgetSpace(EWidgetSpace::Screen);
	QuestMarkerComponent->SetDrawAtDesiredSize(true);
	QuestMarkerComponent->SetPivot(FVector2D(0.5f, 1.f));
	QuestMarkerComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	QuestMarkerComponent->SetVisibility(false);

	InteractionPromptWidgetClass = UInteractionPromptWidget::StaticClass();
}

void ANPCCharacter::BeginPlay()
{
	Super::BeginPlay();

	InteractionSphere->OnComponentBeginOverlap.AddUniqueDynamic(
		this, &ANPCCharacter::HandleInteractionBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddUniqueDynamic(
		this, &ANPCCharacter::HandleInteractionEndOverlap);

	TSubclassOf<UInteractionPromptWidget> PromptWidgetClass =
		InteractionPromptWidgetClass;
	if (!PromptWidgetClass)
	{
		PromptWidgetClass = UInteractionPromptWidget::StaticClass();
	}
	InteractionPromptComponent->SetWidgetClass(PromptWidgetClass);
	InteractionPromptComponent->InitWidget();
	InteractionPromptComponent->SetVisibility(false);
	if (QuestMarkerWidgetClass)
	{
		QuestMarkerComponent->SetWidgetClass(QuestMarkerWidgetClass);
		QuestMarkerComponent->InitWidget();
	}

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

	if (APlayerCharacter* Player = Cast<APlayerCharacter>(
		UGameplayStatics::GetPlayerCharacter(this, 0)))
	{
		BindQuestEvents(Player);
	}
	RefreshQuestMarker();
}

void ANPCCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	InteractionSphere->OnComponentBeginOverlap.RemoveDynamic(
		this, &ANPCCharacter::HandleInteractionBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.RemoveDynamic(
		this, &ANPCCharacter::HandleInteractionEndOverlap);
	if (BoundQuestComponent)
	{
		BoundQuestComponent->OnQuestListChanged.RemoveDynamic(
			this, &ANPCCharacter::RefreshQuestMarker);
	}
	Super::EndPlay(EndPlayReason);
}

void ANPCCharacter::HandleInteractionBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player || OtherComponent != Player->GetCapsuleComponent()) return;

	AOnePlayerController* PlayerController =
		Cast<AOnePlayerController>(Player->GetController());
	if (!PlayerController || !PlayerController->IsLocalController()) return;

	BindQuestEvents(Player);
	Player->SetNearbyInteractable(this);
	SetInteractionPromptVisible(
		IInteractableInterface::Execute_CanInteract(this, Player), Player);
}

void ANPCCharacter::HandleInteractionEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComponent,
	int32 OtherBodyIndex)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (!Player || OtherComponent != Player->GetCapsuleComponent()) return;

	Player->ClearNearbyInteractable(this);
	SetInteractionPromptVisible(false);
}

bool ANPCCharacter::CanInteract_Implementation(APlayerCharacter* Player) const
{
	return IsValid(Player) && IsValid(DialogueData) &&
		InteractionSphere->IsOverlappingComponent(Player->GetCapsuleComponent()) &&
		(bRepeatable || !bDialogueCompleted);
}

void ANPCCharacter::Interact_Implementation(APlayerCharacter* Player)
{
	if (!CanInteract_Implementation(Player)) return;
	SetInteractionPromptVisible(false);
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
	if (IsValid(Player) && IsValid(QuestToOffer))
	{
		if (UQuestComponent* PlayerQuests = Player->GetQuestComponent())
		{
			PlayerQuests->AcceptQuest(QuestToOffer);
		}
	}

	if (IsValid(Player))
	{
		BindQuestEvents(Player);
		RefreshQuestMarker();
		SetInteractionPromptVisible(
			IInteractableInterface::Execute_CanInteract(this, Player), Player);
	}
}

void ANPCCharacter::BindQuestEvents(APlayerCharacter* Player)
{
	UQuestComponent* QuestComponent =
		IsValid(Player) ? Player->GetQuestComponent() : nullptr;
	if (BoundQuestComponent == QuestComponent) return;

	if (BoundQuestComponent)
	{
		BoundQuestComponent->OnQuestListChanged.RemoveDynamic(
			this, &ANPCCharacter::RefreshQuestMarker);
	}
	BoundQuestComponent = QuestComponent;
	if (BoundQuestComponent)
	{
		BoundQuestComponent->OnQuestListChanged.AddUniqueDynamic(
			this, &ANPCCharacter::RefreshQuestMarker);
	}
}

void ANPCCharacter::RefreshQuestMarker()
{
	bool bShouldShow = IsValid(QuestToOffer) && !bDialogueCompleted &&
		QuestMarkerWidgetClass != nullptr;
	if (bShouldShow && BoundQuestComponent)
	{
		bShouldShow = !BoundQuestComponent->HasQuest(QuestToOffer->QuestId);
	}
	QuestMarkerComponent->SetVisibility(bShouldShow);
}

void ANPCCharacter::SetInteractionPromptVisible(
	bool bVisible, APlayerCharacter* Player)
{
	if (!InteractionPromptComponent) return;

	if (bVisible && IsValid(Player))
	{
		if (UInteractionPromptWidget* PromptWidget =
			Cast<UInteractionPromptWidget>(
				InteractionPromptComponent->GetUserWidgetObject()))
		{
			PromptWidget->InitializePrompt(this, Player, InteractionPrompt);
		}
	}

	InteractionPromptComponent->SetVisibility(bVisible);
}
