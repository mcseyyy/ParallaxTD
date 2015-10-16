// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BasicCreep.generated.h"

class ABasicCreep;
class ABasicTower;
class ADebuff;
class UHealthBar;

DECLARE_DELEGATE_OneParam( CreepEventDelegate, ABasicCreep* );
DECLARE_DELEGATE_TwoParams( CreepTowerEventDelegate, ABasicCreep*, ABasicTower* );

UCLASS()
class VENOM_API ABasicCreep : public ACharacter
{
	GENERATED_UCLASS_BODY()
// Engine Functions
public:
	virtual void Tick(float DeltaSeconds) override;
    virtual void BeginPlay() override;
// AI and Behaviour
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Behavior")
    UBehaviorTree* CreepBehaviorTree;
// Combat Functions and Data
public:
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	virtual void OnTakeDamage(int32 DamageReceived, int32& Damage);
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    virtual void OnKilled(ABasicTower* KillingTower);
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    virtual void OnFinished();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CreepAttack();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void InflictDamage(int32 Damage, ABasicTower* AttackingTower);
    UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyDebuff(ADebuff* NewDebuff);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	int32 GetHP();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsAlive();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void KillCreep(ABasicTower* KillingTower);
	void KillCreep(); //for big red button
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Gameplay")
	bool Active;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Gameplay")
	FVector SoundLocation;
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DestroyCreep();
    CreepTowerEventDelegate CreepKilledDelegate;
    CreepEventDelegate CreepFinishedDelegate;
protected:
	TArray<ADebuff*> Debuffs;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Creep Stats")
	bool Alive;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Creep Stats")
	int32 CurrentHP;
public:
    void SetMaxHP(int32 NewMax);
// Base Creep Stats
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Creep Stats")
	FString Name;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Creep Stats")
	int32 Cost;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Creep Stats")
	int32 Income;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Creep Stats")
	int32 Reward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Creep Stats")
	int32 DefaultHP;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Creep Stats")
	float DefaultMoveSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Creep Stats")
	int32 TeamDamage;
    UFUNCTION(BlueprintCallable, Category = "Combat")
    void SetMovementSpeed(float MovementSpeed);
// Player/Team Data
public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player Data")
	int32 OwnerID;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Player Data")
	int32 EnemyTeamID;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Stuff")
	bool FinishedInit = false;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health Bar")
	TSubclassOf<UHealthBar> HealthBarType;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health Bar")
	UHealthBar* HealthBar;
};
