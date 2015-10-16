// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "BasicTower.h"
#include "BasicCreep.h"
#include "TowerBuff.h"
#include "Engine.h"
#include "BasicTowerAIController.h"

ABasicTower::ABasicTower(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	AIControllerClass = ABasicTowerAIController::StaticClass();
	SceneComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Scene Component"));
	RootComponent = SceneComponent;
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> DefaultBehavior(
        TEXT("BehaviorTree'/Game/Blueprints/Towers/TowerAI/BasicTowerBehavior.BasicTowerBehavior'"));
	if (DefaultBehavior.Object != NULL) TowerBehaviorTree = DefaultBehavior.Object;
	else DEBUG_MESSAGE("Could not find tower behavior tree.");
	FinishedSpawning = false;
	BeingRemoved = false;
	bCollideWhenPlacing = true;
    TimeUntilAttack = 0;
}

//Tick
//Update the fire cool down by decrementing it
void ABasicTower::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!ControllerSpawned)
		if (FinishedSpawning)
		{
			SpawnDefaultController();
			ControllerSpawned = true;
		}
	for (auto Buff : Buffs){
		Buff->BuffTick(DeltaSeconds);
	}
	Buffs.RemoveAll(
		[](ATowerBuff* Buff){
        if (Buff->IsPendingKill()){
            DEBUG_MESSAGE("Removing Buff");
            return true;
        };
        return false;
	});
    if (TimeUntilAttack > 0) TimeUntilAttack -= DeltaSeconds;
}

void ABasicTower::DestroyTower()
{
	for (auto Buff : Buffs){
		Buff->Destroy();
	}
	CollisionBox = NULL;
	Destroy();
}

bool ABasicTower::TargetIsValid(ABasicCreep *Target)
{
    if (!Target
        || Target->IsPendingKill()
        || !WithinRange(Target)) return false;
    return true;
}

void ABasicTower::StartRemove()
{
	BeingRemoved = true;
	OnRemove();
}

void ABasicTower::CancelRemove()
{
	BeingRemoved = false;
	OnRemoveCancel();
}

void ABasicTower::FinishRemoval()
{
	if (BeingRemoved){
		TowerRemovedDelegate.Execute(this);
		DestroyTower();
	}
}

//Within Range
//Calculates the distance between the tower and a given creep
//If the distance is <= the tower's range, the creep is in range
bool ABasicTower::WithinRange(ABasicCreep *Target)
{
	UCapsuleComponent *Capsule = Target->GetCapsuleComponent();
    float Distance;
    if (Capsule){
	    FVector Difference = Capsule->GetComponentLocation()
						     - RootComponent->GetComponentLocation();
	    Difference.Z = 0.f;
	    Distance = Difference.Size() - Capsule->GetScaledCapsuleRadius();
	    if (Distance <= MaxRange) return true;
    }
	return false;
}

ABasicCreep* ABasicTower::SelectTarget_Implementation(const TArray<ABasicCreep*>& Targets)
{
	return Targets[0];
}

bool ABasicTower::FireAtTarget(ABasicCreep* Target)
{
	bool Result = false;
	ABasicTowerProjectile* Projectile = NULL;
	if (ProjectileType){
		UWorld *const World = GetWorld();
		if (World){
			FVector SpawnLoc = RootComponent->GetComponentLocation();
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			Projectile = World->SpawnActor<ABasicTowerProjectile>(
				ProjectileType, SpawnLoc, FRotator::ZeroRotator, SpawnParams);
			if (Projectile){
				Projectile->FireProjectile(Target, this);
                Result = true;
			}
		}
	}
	else Result = true;
    if (Result){
        OnFire(Target, Projectile);
        TimeUntilAttack = AttackCooldown;
    }
    return Result;
}

void ABasicTower::ApplyBuff(ATowerBuff* NewBuff)
{
	for (auto Buff : Buffs){
		if (Buff->IsA(NewBuff->StaticClass()) || NewBuff->IsA(Buff->StaticClass())){
			Buff->RefreshBuff(NewBuff);
			NewBuff->Destroy();
			return;
		}
	}
	Buffs.Add(NewBuff);
	NewBuff->ActivateBuff(this);
}
