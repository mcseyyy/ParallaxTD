// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "BasicCreep.h"
#include "BasicTowerProjectile.h"
#include "Debuff.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BasicTower.generated.h"

DECLARE_DELEGATE_OneParam(TowerEventDelegate, ABasicTower*);

class ABasicCreep;
class ATowerBuff;

UCLASS()
class VENOM_API ABasicTower : public APawn
{
	GENERATED_BODY()
// Engine Functions
public:
    ABasicTower(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds) override;
// AI and Behaviour
public:
	UFUNCTION(BlueprintCallable, Category = "Behavior")
    bool TargetIsValid(ABasicCreep *Target);
	UFUNCTION(BlueprintCallable, Category = "Behavior")
	bool FireAtTarget(ABasicCreep* Target);
    UFUNCTION(BlueprintNativeEvent, Category = "Behavior")
	ABasicCreep* SelectTarget(const TArray<ABasicCreep*>& Targets);
	virtual ABasicCreep* SelectTarget_Implementation(const TArray<ABasicCreep*>& Targets);
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Behavior")
    UBehaviorTree* TowerBehaviorTree;
protected:
    bool WithinRange(ABasicCreep* Target);
// Base Tower Stats
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tower Stats")
    FString Name;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tower Stats")
    int32 Cost;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tower Stats")
	float MaxRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tower Stats")
    float AttackCooldown;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tower Stats")
	int32 Damage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Tower Stats")
	TSubclassOf<ABasicTowerProjectile> ProjectileType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stuff")
	float SpawnTime = 5.0;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Stuff")
	bool FinishedSpawning;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Stuff")
	bool BeingRemoved;
// Combat Functions & Data
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Team Data")
    float TimeUntilAttack;
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DestroyTower();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyBuff(ATowerBuff* NewBuff);
protected:
	TArray<ATowerBuff*> Buffs;
// Player/Team Data
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Team Data")
    int32 OwnerID;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Team Data")
	int32 TeamID;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Team Data")
	int32 TowerID;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Gameplay")
	FVector SoundLocation;
// Blueprint Implemented Events
public:
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    virtual void OnHitCreep(ABasicCreep* Target);
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    virtual void OnDamageCreep(ABasicCreep* Target, int32 DamageDealt, int32 RemainingHP);
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	virtual void OnKillCreep(ABasicCreep* Target);
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	virtual void OnFire(ABasicCreep* Target, ABasicTowerProjectile* Projectile);
	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	virtual void OnRemove();
	UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	virtual void OnRemoveCancel();
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void FinishRemoval();
	TowerEventDelegate TowerRemovedDelegate;
	void StartRemove();
	void CancelRemove();
// Components
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower Collision")
	UBoxComponent* CollisionBox;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scene Component")
	USceneComponent* SceneComponent; // serves as the rootof the object
private:
	bool ControllerSpawned = false;
};
