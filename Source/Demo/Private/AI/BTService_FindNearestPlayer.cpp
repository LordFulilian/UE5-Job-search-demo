// Rylan


#include "AI/BTService_FindNearestPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	APawn* OwningPawn = AIOwner->GetPawn(); 
	
	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player"))? FName("Enemy"): FName("Player");
	
	TArray<AActor*> ActorsWithTag;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn,TargetTag,ActorsWithTag);
}
