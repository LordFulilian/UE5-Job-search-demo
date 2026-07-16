#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Character/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"


UBTService_FindNearestPlayer::UBTService_FindNearestPlayer()
{
    NodeName = TEXT("Find Nearest Player");
    INIT_SERVICE_NODE_NOTIFY_FLAGS();
    bCallTickOnSearchStart = true;
    Interval = 0.25f;
    RandomDeviation = 0.05f;

    TargetToFollowSelector.AddObjectFilter(
        this,
        GET_MEMBER_NAME_CHECKED(
            UBTService_FindNearestPlayer,
            TargetToFollowSelector),
        AActor::StaticClass());

    DistanceToTargetSelector.AddFloatFilter(
        this,
        GET_MEMBER_NAME_CHECKED(
            UBTService_FindNearestPlayer,
            DistanceToTargetSelector));

    HomeLocationSelector.AddVectorFilter(
        this,
        GET_MEMBER_NAME_CHECKED(
            UBTService_FindNearestPlayer,
            HomeLocationSelector));

    PatrolLocationSelector.AddVectorFilter(
        this,
        GET_MEMBER_NAME_CHECKED(
            UBTService_FindNearestPlayer,
            PatrolLocationSelector));

    ReturningHomeSelector.AddBoolFilter(
        this,
        GET_MEMBER_NAME_CHECKED(
            UBTService_FindNearestPlayer,
            ReturningHomeSelector));
}

void UBTService_FindNearestPlayer::TickNode(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory,
    float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AAIController* AIController = OwnerComp.GetAIOwner();
    APawn* OwningPawn = AIController ? AIController->GetPawn() : nullptr;
    AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(OwningPawn);
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Enemy || !Blackboard || Enemy->bIsDead)
    {
        return;
    }

    const FVector HomeLocation = Enemy->GetHomeLocation();
    const float DistanceFromHome =
        FVector::Dist(Enemy->GetActorLocation(), HomeLocation);

    Blackboard->SetValueAsVector(
        HomeLocationSelector.SelectedKeyName,
        HomeLocation);

    AActor* CurrentTarget =
        Cast<AActor>(
            Blackboard->GetValueAsObject(
                TargetToFollowSelector.SelectedKeyName));

    const float TargetDistance =
        IsValid(CurrentTarget)
        ? Enemy->GetDistanceTo(CurrentTarget)
        : TNumericLimits<float>::Max();

    const bool bExceededChaseDistance =
        IsValid(CurrentTarget) &&
        (DistanceFromHome > Enemy->GetMaxChaseDistance() ||
         TargetDistance > Enemy->GetMaxChaseDistance());

    if (bExceededChaseDistance)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("[EnemyAI] %s exceeded chase range: home=%.1f target=%.1f max=%.1f"),
            *Enemy->GetName(),
            DistanceFromHome,
            TargetDistance,
            Enemy->GetMaxChaseDistance());

        Blackboard->SetValueAsObject(
            TargetToFollowSelector.SelectedKeyName,
            nullptr);

        Blackboard->SetValueAsFloat(
            DistanceToTargetSelector.SelectedKeyName,
            TNumericLimits<float>::Max());

        Blackboard->SetValueAsBool(
            ReturningHomeSelector.SelectedKeyName,
            true);

        Blackboard->SetValueAsVector(
            PatrolLocationSelector.SelectedKeyName,
            HomeLocation);

        return;
    }

    const bool bReturningHome =
        Blackboard->GetValueAsBool(
            ReturningHomeSelector.SelectedKeyName);

    if (bReturningHome)
    {
        if (DistanceFromHome >
            Enemy->GetHomeAcceptanceRadius())
        {
            Blackboard->SetValueAsObject(
                TargetToFollowSelector.SelectedKeyName,
                nullptr);

            Blackboard->SetValueAsVector(
                PatrolLocationSelector.SelectedKeyName,
                HomeLocation);

            return;
        }

        Blackboard->SetValueAsBool(
            ReturningHomeSelector.SelectedKeyName,
            false);

        UE_LOG(
            LogTemp,
            Display,
            TEXT("[EnemyAI] %s returned to chase origin"),
            *Enemy->GetName());
    }

    // 已经有目标时，不要因为其他玩家更近而频繁切换目标。
    if (IsValid(CurrentTarget) &&
        TargetDistance <= Enemy->GetMaxChaseDistance())
    {
        Blackboard->SetValueAsFloat(
            DistanceToTargetSelector.SelectedKeyName,
            TargetDistance);

        return;
    }

    const FName TargetTag =
        OwningPawn->ActorHasTag(FName("Player"))
        ? FName("Enemy")
        : FName("Player");

    TArray<AActor*> ActorsWithTag;
    UGameplayStatics::GetAllActorsWithTag(
        OwningPawn,
        TargetTag,
        ActorsWithTag);

    float ClosestDistance =
        TNumericLimits<float>::Max();

    AActor* ClosestActor = nullptr;

    for (AActor* Actor : ActorsWithTag)
    {
        if (!IsValid(Actor))
        {
            continue;
        }

        const float Distance =
            OwningPawn->GetDistanceTo(Actor);

        if (Distance <= Enemy->GetAggroRadius() &&
            Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            ClosestActor = Actor;
        }
    }

    if (IsValid(ClosestActor))
    {
        const FVector ChaseOrigin = Enemy->GetActorLocation();
        Enemy->SetHomeLocation(ChaseOrigin);
        Blackboard->SetValueAsVector(
            HomeLocationSelector.SelectedKeyName,
            ChaseOrigin);

        UE_LOG(
            LogTemp,
            Display,
            TEXT("[EnemyAI] %s acquired %s at %s, max chase=%.1f"),
            *Enemy->GetName(),
            *ClosestActor->GetName(),
            *ChaseOrigin.ToCompactString(),
            Enemy->GetMaxChaseDistance());
    }

    Blackboard->SetValueAsObject(
        TargetToFollowSelector.SelectedKeyName,
        ClosestActor);

    Blackboard->SetValueAsFloat(
        DistanceToTargetSelector.SelectedKeyName,
        ClosestDistance);
}
