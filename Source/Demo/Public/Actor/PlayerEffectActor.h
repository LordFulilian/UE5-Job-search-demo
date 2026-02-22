

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "PlayerEffectActor.generated.h"

class USphereComponent;

UCLASS()
class DEMO_API APlayerEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	
	APlayerEffectActor();

	

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor,TSubclassOf<UGameplayEffect>GameplayEffectClass);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applying Effect")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
};
