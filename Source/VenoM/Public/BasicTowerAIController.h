// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AIController.h"
#include "BasicTower.h"
#include "BasicCreep.h"
#include "Battlefield.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BasicTowerAIController.generated.h"

UCLASS()
class VENOM_API ABasicTowerAIController : public AAIController
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Possess(APawn* InPawn) override;
    UFUNCTION(BlueprintCallable, Category = "Behavior")
    ABasicTower* GetTower();
    UFUNCTION(BlueprintCallable, Category = "Behavior")
    ABasicCreep* GetBestTarget();
protected:
	UPROPERTY(transient)
    UBlackboardComponent* Blackboard;
	UPROPERTY(transient)
    UBehaviorTreeComponent* BehaviorTree;
	ABasicTower* PossessedTower;
	ABattlefield* OwningTeam;
    /*
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Behavior")
	TArray<ABasicCreep*> AvailableTargets;
	USphereComponent* RangeSphere;
	UFUNCTION()
	void OnOverlapBegin(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
    */
};
