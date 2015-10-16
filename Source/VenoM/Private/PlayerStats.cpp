// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "PlayerStats.h"

void APlayerStats::InitPlayer(int32 StartMoney, int32 StartIncome)
{
    SetMoney(StartMoney);
    SetIncome(StartIncome);
    Kills = 0;
}

void APlayerStats::WaveTick()
{
    AddMoney(Income);
}

bool APlayerStats::CanPurchaseSelectedTower()
{
	if (!SelectedTower)
		return false;
    int32 Cost = SelectedTower->GetDefaultObject<ABasicTower>()->Cost;
	if (Cost > Money)
		return false;
    return true;
}
bool APlayerStats::CanPurchaseCreep(TSubclassOf<ABasicCreep> Creep)
{
    int32 Cost = Creep->GetDefaultObject<ABasicCreep>()->Cost;
    if (Cost > Money) return false;
    return true;
}
void APlayerStats::FinalizePurchaseSelectedTower()
{
    if (!SelectedTower) return;
    AddMoney(-(SelectedTower->GetDefaultObject<ABasicTower>()->Cost));
}
void APlayerStats::FinalizePurchaseCreep(TSubclassOf<ABasicCreep> Creep)
{
    AddMoney(-(Creep->GetDefaultObject<ABasicCreep>()->Cost));
    AddIncome(Creep->GetDefaultObject<ABasicCreep>()->Income);
}

void APlayerStats::SetMoney(int32 NewMoney)
{
    if (NewMoney < 0) NewMoney = 0;
    if (Money != NewMoney){
        Money = NewMoney;
        MoneyChanged = true;
    }
}

void APlayerStats::AddMoney(int32 MoneyAdded)
{
    SetMoney(Money + MoneyAdded);
}

int32 APlayerStats::GetMoney(){ return Money; }

void APlayerStats::SetIncome(int32 NewIncome)
{
    if (NewIncome < 0) NewIncome = 0;
    if (Income != NewIncome){
        Income = NewIncome;
        IncomeChanged = true;
    }
}

void APlayerStats::AddIncome(int32 IncomeAdded)
{
    SetIncome(Income + IncomeAdded);
}

int32 APlayerStats::GetIncome(){ return Income; }

bool APlayerStats::MoneyChangedSinceLastCheck()
{
    if (MoneyChanged){
        MoneyChanged = false;
        return true;
    }
    else return false;
}

bool APlayerStats::IncomeChangedSinceLastCheck()
{
    if (IncomeChanged){
        IncomeChanged = false;
        return true;
    }
    else return false;
}
