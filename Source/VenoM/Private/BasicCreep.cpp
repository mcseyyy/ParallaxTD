// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "BasicCreep.h"
#include "Engine.h"
#include "BasicTower.h"
#include "Debuff.h"
#include "HealthBar.h"
#include "BasicCreepAIController.h"


ABasicCreep::ABasicCreep(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    AIControllerClass = ABasicCreepAIController::StaticClass();
    GetCapsuleComponent()->BodyInstance.SetCollisionProfileName("Creep");
    DefaultHP = 200;
    DefaultMoveSpeed = 600.f;
	TeamDamage = 1;
	//StaticMesh = ObjectInitializer.CreateDefaultSubobject<UStaticMesh>(this, TEXT("Static Mesh"));
	//HealthBar = CreateWidget<UHealthBar>(playerController, HealthBarType);
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> DefaultBehavior(
        TEXT("BehaviorTree'/Game/Blueprints/Creeps/CreepAI/BasicCreepBehavior.BasicCreepBehavior'"));
	if (DefaultBehavior.Object != NULL) CreepBehaviorTree = DefaultBehavior.Object;
	else DEBUG_MESSAGE("Could not find creep behavior tree.");
}

// Begin Play
// Sets the HP and Movement Speed of the creep to their default values
void ABasicCreep::BeginPlay()
{
    Super::BeginPlay();
	CurrentHP = DefaultHP;
    SetMovementSpeed(DefaultMoveSpeed);
    Alive = true;
	Active = true;
}

// Tick
// Purges expired debuffs and calls Execute Debuffs
void ABasicCreep::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	
	if (!Active) DestroyCreep();
    for (auto Debuff : Debuffs){
        Debuff->DebuffTick(DeltaSeconds);
    }
    Debuffs.RemoveAll(
        [](ADebuff* Debuff){
            return Debuff->IsPendingKill();
        });
}

void ABasicCreep::SetMaxHP(int32 NewMax)
{
    float Ratio = (float)NewMax / DefaultHP;
    DefaultHP = NewMax;
    CurrentHP = Ratio * CurrentHP;
}

void ABasicCreep::SetMovementSpeed(float MovementSpeed)
{
    GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
}

void ABasicCreep::DestroyCreep()
{
	for (auto Debuff : Debuffs){
		Debuff->Destroy();
	}
	Destroy();
}

void ABasicCreep::CreepAttack()
{
	Alive = false;
	Active = false;
    OnFinished();
    CreepFinishedDelegate.Execute(this);
}

// Inflict Damage
// Causes the creep to take damage from the player given by PlayerID
// The creep will handle its own death if the damage is sufficient to kill it,
// including rewarding the responsible player
void ABasicCreep::InflictDamage(int32 Damage, ABasicTower* AttackingTower)
{
    OnTakeDamage(Damage, Damage);
    if (Damage > 0){
        CurrentHP -= Damage;
        if(AttackingTower) AttackingTower->OnDamageCreep(this, Damage, CurrentHP);
        if (CurrentHP <= 0) KillCreep(AttackingTower);
    }
}



bool ABasicCreep::IsAlive()
{
    return Alive;
}

void ABasicCreep::KillCreep(ABasicTower* KillingTower)
{
	Alive = false;
	Active = false;
    OnKilled(KillingTower);
    if (!Alive){
        if (KillingTower) KillingTower->OnKillCreep(this);
        CreepKilledDelegate.Execute(this, KillingTower);
    }
}

void ABasicCreep::KillCreep()
{
	Alive = false;
	Active = false;
	OnKilled(nullptr);
}



int32 ABasicCreep::GetHP()
{
    return CurrentHP;
}

// Apply Debuff
// Applies a given debuff to the creep
// If the creep already has a debuff of the same type affecting it, then the
// old debuff is replaced if it is weaker or refreshed to the duration of the
// new buff if it has the same strength
void ABasicCreep::ApplyDebuff(ADebuff* NewDebuff)
{
    for (auto Debuff : Debuffs){
        if (Debuff->StaticClass() == NewDebuff->StaticClass()){
            Debuff->RefreshDebuff(NewDebuff);
            NewDebuff->Destroy();
            return;
        }
    }
    Debuffs.Add(NewDebuff);
    NewDebuff->ActivateDebuff(this);
}
