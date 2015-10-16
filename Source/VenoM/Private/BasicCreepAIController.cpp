// Copyright VenoM Inc. 2014-2015

#include "VenoM.h"
#include "BasicCreepAIController.h"
#include "PlayGameMode.h"

ABasicCreepAIController::ABasicCreepAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
 	Blackboard = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("Blackboard"));
 	BehaviorTree = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTree"));
    WaitingOnAsyncQuery = false;
}

void ABasicCreepAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	ABasicCreep *Creep = Cast<ABasicCreep>(InPawn);
	if (Creep){
		PossessedCreep = Creep;
        APlayGameMode* GameMode = GetWorld()->GetAuthGameMode<APlayGameMode>();
        if (GameMode){
            EnemyTeam = GameMode->GetTeam(Creep->EnemyTeamID);
        }
        if (Creep->CreepBehaviorTree){
            Blackboard->InitializeBlackboard(*(Creep->CreepBehaviorTree->BlackboardAsset));
            Blackboard->SetValueAsObject(
                FName("Target Base"), EnemyTeam->CreepDestinationPoints[0]);
            Blackboard->SetValueAsBool(FName("IsStunned"), false);
            BehaviorTree->StartTree(*(Creep->CreepBehaviorTree));
        }
        else DEBUG_MESSAGE(TEXT("Warning: Creep spawned with no behavior tree set."));
		AsyncPathDelegate.BindUObject(this, &ABasicCreepAIController::AsyncPathFound);
	}
	else{
		UnPossess();
        Destroy();
	}
}

bool ABasicCreepAIController::MoveCreepToTarget(AActor* Target)
{
	if (FVector::Dist(GetCreep()->GetActorLocation(), Target->GetActorLocation())
		<= 200 + 80) return true;
    MoveToLocation(Target->GetActorLocation(), 100, false, true, false);
	DEBUG_MESSAGE(*(GetPathFollowingComponent()->GetDebugString()));
    return false;
}

bool ABasicCreepAIController::MoveCreepToTargetAsync(AActor* Target)
{
	if (FVector::Dist(GetCreep()->GetActorLocation(), Target->GetActorLocation())
		<= 200 + 80) return true;
    if (WaitingOnAsyncQuery) return false;
    FPathFindingQuery Query;
    PreparePathfinding(Query, Target->GetActorLocation(), NULL);
    GetWorld()->GetNavigationSystem()->FindPathAsync(FNavAgentProperties(72.0, 72.0), Query, AsyncPathDelegate, EPathFindingMode::Hierarchical);
    WaitingOnAsyncQuery = true;
    return false;
}

void ABasicCreepAIController::AsyncPathFound(uint32 Val, ENavigationQueryResult::Type Result, FNavPathSharedPtr NavPath)
{
    WaitingOnAsyncQuery = false;
    if (Result != ENavigationQueryResult::Success){
        //DEBUG_MESSAGE("Async pathfinding failed.");
        return;
    }
    RequestMove(NavPath, NULL, 100);
}

ABasicCreep* ABasicCreepAIController::GetCreep()
{
	return PossessedCreep;
}


