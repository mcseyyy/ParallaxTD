// Copyright VenoM Inc. 2014-2015

#pragma once

#include "AIController.h"
#include "BasicCreep.h"
#include "Battlefield.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BasicCreepAIController.generated.h"

UCLASS()
class VENOM_API ABasicCreepAIController : public AAIController
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Possess(APawn* InPawn) override;
    UFUNCTION(BlueprintCallable, Category = "Behavior")
    ABasicCreep* GetCreep();
    UFUNCTION(BlueprintCallable, Category = "Behavior")
	bool MoveCreepToTarget(AActor* Target);
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	bool MoveCreepToTargetAsync(AActor* Target);
protected:
	UPROPERTY(VisibleInstanceOnly, transient)
    UBlackboardComponent* Blackboard;
	UPROPERTY(transient)
    UBehaviorTreeComponent* BehaviorTree;
    void AsyncPathFound(uint32 Val, ENavigationQueryResult::Type Result, FNavPathSharedPtr NavPath);
    bool WaitingOnAsyncQuery;
    FNavPathQueryDelegate AsyncPathDelegate;
    ABasicCreep* PossessedCreep;
	ABattlefield* EnemyTeam;
};
