#include "AI/BTTask_SelectPatrolPoint.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/EnemyCharacter.h"

UBTTask_SelectPatrolPoint::UBTTask_SelectPatrolPoint()
{
	NodeName = TEXT("Select Next Patrol Point");
	PatrolLocationSelector.AddVectorFilter(
	   this,
	   GET_MEMBER_NAME_CHECKED(
		   UBTTask_SelectPatrolPoint,
		   PatrolLocationSelector));
}

EBTNodeResult::Type UBTTask_SelectPatrolPoint::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	AAIController* AIController =
		OwnerComp.GetAIOwner();

	AEnemyCharacter* Enemy =
		AIController
		? Cast<AEnemyCharacter>(AIController->GetPawn())
		: nullptr;

	UBlackboardComponent* Blackboard =
		OwnerComp.GetBlackboardComponent();

	if (!Enemy || !Blackboard || Enemy->bIsDead)
	{
		return EBTNodeResult::Failed;
	}

	if (PatrolLocationSelector.SelectedKeyName.IsNone())
	{
		return EBTNodeResult::Failed;
	}

	const FVector NextPatrolLocation =
		Enemy->SelectNextPatrolLocation();

	Blackboard->SetValueAsVector(
		PatrolLocationSelector.SelectedKeyName,
		NextPatrolLocation);

	return EBTNodeResult::Succeeded;
}