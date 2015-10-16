// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "Debuff.h"
#include "BasicCreep.h"


ADebuff::ADebuff(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = false;
    Active = false;
}

void ADebuff::DebuffTick(float DeltaSeconds)
{
    ElapsedTime += DeltaSeconds;
    if (ElapsedTime >= FinishTime){
        if (FinishTime == NextProc) DebuffProc();
        FinishDebuff();
    }
    else if (ElapsedTime >= NextProc && NextProc != -1){
        DebuffProc();
        if(ProcRate > 0) NextProc += ProcRate;
    }
}

void ADebuff::SetRemainingTime(float RemainingTime)
{
    FinishTime = ElapsedTime + RemainingTime;
}

bool ADebuff::ActivateDebuff(ABasicCreep* Victim)
{
    if(Active) return false;
    Target = Victim;
    ElapsedTime = 0;
    NextProc = 0;
    FinishTime = (Duration >= 0) ? Duration : -1;
    Active = true;
	OnDebuffStart();
    return true;
}

void ADebuff::FinishDebuff()
{
    Active = false;
    OnDebuffEnd();
    if(!Active) Destroy();
}

void ADebuff::ProcDebuff()
{
    DebuffProc();
}

void ADebuff::RefreshDebuff_Implementation(ADebuff* NewDebuff)
{
    if (NewDebuff->Duration > (FinishTime - ElapsedTime)) SetRemainingTime(NewDebuff->Duration);
}
