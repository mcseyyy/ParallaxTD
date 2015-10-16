// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BasicCreep.h"
#include "BasicTower.h"
#include "PlayerStats.generated.h"

UCLASS()
class VENOM_API APlayerStats : public AInfo
{
	GENERATED_BODY()
// Gameplay Functions
public:
    void InitPlayer(int32 StartMoney, int32 StartIncome);
    void WaveTick();
    bool CanPurchaseSelectedTower();
    bool CanPurchaseCreep(TSubclassOf<ABasicCreep> Creep);
    void FinalizePurchaseSelectedTower();
    void FinalizePurchaseCreep(TSubclassOf<ABasicCreep> Creep);
    void SelectTower(TSubclassOf<ABasicTower> Selection){ SelectedTower = Selection; }
// Player Stats
public:
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
    int32 Kills;
    TSubclassOf<ABasicTower> SelectedTower;
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void SetMoney(int32 NewMoney);
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void AddMoney(int32 MoneyAdded);
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void SetIncome(int32 NewIncome);
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void AddIncome(int32 IncomeAdded);
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    int32 GetMoney();
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    int32 GetIncome();
    bool MoneyChangedSinceLastCheck();
    bool IncomeChangedSinceLastCheck();
protected:
    int32 Money;
    int32 Income;
    bool MoneyChanged;
    bool IncomeChanged;
};