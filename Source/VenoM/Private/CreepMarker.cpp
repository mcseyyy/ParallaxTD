// Copyright VenoM Inc. 2014-2015

#include "VenoM.h"
#include "CreepMarker.h"

void ACreepMarker::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	MaterialPower = FMath::Max(MaterialPower - DeltaSeconds / CoolDownTime, 0.f);

}

void ACreepMarker :: SpawnCreep()
{
	MaterialPower = FMath::Min(1.f, MaterialPower + PowerIncPerCreep);
}



