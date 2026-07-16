// Rylan

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UBTService_FindNearestPlayer : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_FindNearestPlayer();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	FBlackboardKeySelector TargetToFollowSelector;

	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	FBlackboardKeySelector DistanceToTargetSelector;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HomeLocationSelector;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolLocationSelector;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector ReturningHomeSelector;
};
