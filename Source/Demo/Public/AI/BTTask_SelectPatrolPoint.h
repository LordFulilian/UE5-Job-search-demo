#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "BTTask_SelectPatrolPoint.generated.h"

UCLASS()
class DEMO_API UBTTask_SelectPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SelectPatrolPoint();

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolLocationSelector;
};