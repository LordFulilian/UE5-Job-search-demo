// Rylan


#include "AI/BTTask_Attack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/BossCharacter.h"

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	ABossCharacter* Boss =
		AIController ? Cast<ABossCharacter>(AIController->GetPawn()) : nullptr;
	if (Boss)
	{
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		AActor* Target = Blackboard
			? Cast<AActor>(Blackboard->GetValueAsObject(TEXT("TargetToFollow")))
			: nullptr;
		if (!IsValid(Target) ||
			FVector::Dist2D(Boss->GetActorLocation(), Target->GetActorLocation()) >
			Boss->GetBossAttackRange())
		{
			return EBTNodeResult::Failed;
		}

		AIController->StopMovement();
		FVector DirectionToTarget =
			Target->GetActorLocation() - Boss->GetActorLocation();
		DirectionToTarget.Z = 0.f;
		if (!DirectionToTarget.IsNearlyZero())
		{
			Boss->SetActorRotation(DirectionToTarget.Rotation());
		}

		return Boss->TryActivateBossAbility()
			? EBTNodeResult::Succeeded
			: EBTNodeResult::Failed;
	}

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
