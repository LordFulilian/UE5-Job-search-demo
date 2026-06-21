// Rylan

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PlayerAIController.generated.h"


class UBlackboardComponent;
class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class DEMO_API APlayerAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	APlayerAIController();
	
protected:	

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
