// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "Engine.h"
#include "BasicTowerAIController.h"
#include "PlayGameMode.h"

ABasicTowerAIController::ABasicTowerAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    /*
	RangeSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	RangeSphere->InitSphereRadius(0.f);
	RangeSphere->SetCollisionProfileName("OverlapAll");
	//RangeSphere->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryOnly, true);
	//RangeSphere->BodyInstance.SetResponseToChannel(COLLISION_CREEP, ECR_Overlap);
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ABasicTowerAIController::OnOverlapBegin);
	RangeSphere->OnComponentEndOverlap.AddDynamic(this, &ABasicTowerAIController::OnOverlapEnd);
    */
	Blackboard = ObjectInitializer.CreateDefaultSubobject<UBlackboardComponent>(this, TEXT("Blackboard"));
	BehaviorTree = ObjectInitializer.CreateDefaultSubobject<UBehaviorTreeComponent>(this, TEXT("BehaviorTree"));
}

void ABasicTowerAIController::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	ABasicTower *Tower = Cast<ABasicTower>(InPawn);
	if (Tower){
		PossessedTower = Tower;
        APlayGameMode* GameMode = GetWorld()->GetAuthGameMode<APlayGameMode>();
        if (GameMode){
            OwningTeam = GameMode->GetTeam(Tower->TeamID);
        }
        if (Tower->TowerBehaviorTree){
            Blackboard->InitializeBlackboard(*(Tower->TowerBehaviorTree->BlackboardAsset));
            BehaviorTree->StartTree(*(Tower->TowerBehaviorTree));
        }
        else DEBUG_MESSAGE(TEXT("Warning: Tower spawned with no behavior tree set."));
		//RangeSphere->SetSphereRadius(Tower->MaxRange);
	}
	else{
		UnPossess();
        Destroy();
	}
}

    /*
void ABasicTowerAIController::OnOverlapBegin(AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DEBUG_MESSAGE("Tower Overlap");
	ABasicCreep* NewTarget = Cast<ABasicCreep>(OtherActor);
	if (NewTarget){
		if (NewTarget->EnemyTeamID == PossessedTower->TeamID){
			DEBUG_MESSAGE("New Target");
			AvailableTargets.Add(NewTarget);
		}
	}
}

void ABasicTowerAIController::OnOverlapEnd(AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	DEBUG_MESSAGE("Tower Overlap End");
	ABasicCreep* NewTarget = Cast<ABasicCreep>(OtherActor);
	if (NewTarget){
		if (NewTarget->EnemyTeamID == PossessedTower->TeamID){
			DEBUG_MESSAGE("Lost Target");
			AvailableTargets.RemoveSingle(NewTarget);
		}
	}
}
    */

ABasicTower* ABasicTowerAIController::GetTower()
{
	return PossessedTower;
}

ABasicCreep* ABasicTowerAIController::GetBestTarget()
{
    ABasicCreep* BestTarget = NULL;
	TArray<FOverlapResult> Overlaps;
    GetWorld()->OverlapMulti(Overlaps,PossessedTower->GetActorLocation(),FQuat::Identity,
                             FCollisionShape::MakeSphere(PossessedTower->MaxRange),
                             FCollisionQueryParams(TEXT("TestThing"), false, this),
                             FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllDynamicObjects));
    for (auto Overlap : Overlaps){
        BestTarget = Cast<ABasicCreep>(Overlap.GetActor());
        if (BestTarget && BestTarget->EnemyTeamID == PossessedTower->TeamID) return BestTarget;
    }
    return NULL;
}
