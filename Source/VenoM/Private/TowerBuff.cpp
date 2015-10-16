// Copyright VenoM Inc. 2014-2015

#include "VenoM.h"
#include "TowerBuff.h"


// Sets default values
ATowerBuff::ATowerBuff()
{
	PrimaryActorTick.bCanEverTick = false;
    Active = false;
}

void ATowerBuff::BuffTick(float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;
	if (ElapsedTime >= FinishTime){
		if (FinishTime == NextProc) BuffProc();
		FinishBuff();
	}
	else if (ElapsedTime >= NextProc && NextProc != -1){
		BuffProc();
		if (ProcRate > 0) NextProc += ProcRate;
	}
}

void ATowerBuff::SetRemainingTime(float RemainingTime)
{
	FinishTime = ElapsedTime + RemainingTime;
}

bool ATowerBuff::ActivateBuff(ABasicTower* TargetTower)
{
	if (Active) return false;
	Target = TargetTower;
	ElapsedTime = 0;
	NextProc = 0;
	FinishTime = (Duration >= 0) ? Duration : -1;
    Active = true;
	OnBuffStart();
	return true;
}

void ATowerBuff::FinishBuff()
{
	Active = false;
	OnBuffEnd();
    if(!Active) Destroy();
}

void ATowerBuff::ProcBuff()
{
	BuffProc();
}

void ATowerBuff::RefreshBuff_Implementation(ATowerBuff* NewBuff)
{
	if (NewBuff->Duration > (FinishTime - ElapsedTime)) SetRemainingTime(NewBuff->Duration);
}

void ATowerBuff::BeginPlay()
{
	Super::BeginPlay();
	
}


