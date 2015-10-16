// Copyright VenoM Inc. 2014-2015

#include "VenoM.h"
#include "PlayGameMode.h"
#include "BasicTower.h"
#include "BasicCreep.h"
#include "Battlefield.h"
#include "AreaOfEffect.h"


AAreaOfEffect::AAreaOfEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AAreaOfEffect::BeginPlay()
{
	Super::BeginPlay();
    Active = false;
}

void AAreaOfEffect::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
    if (!Active) return;
    if (TargetUnit){
        if (TargetUnit->IsPendingKill()) Destroy();
        else SetActorLocation(TargetUnit->GetRootComponent()->GetComponentLocation());
    }
    if (OwnerTower && OwnerTower->IsPendingKill()) OwnerTower = NULL;
    ElapsedTime += DeltaTime;
    if (ElapsedTime >= FinishTime){
        if (FinishTime == NextProc){
            ProcCreeps();
        }
        Destroy();
        return;
    }
    if (ProcRate > 0 && ElapsedTime >= NextProc){
        ProcCreeps();
        NextProc += ProcRate;
    }
}

void AAreaOfEffect::ProcCreeps()
{
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMulti(Overlaps, GetActorLocation(), FQuat::Identity,
		FCollisionShape::MakeSphere(Radius),
		FCollisionQueryParams(TEXT("TestThing"), false, this),
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllDynamicObjects));
	for (auto Overlap : Overlaps){
		ABasicCreep* NextTarget = Cast<ABasicCreep>(Overlap.GetActor());
		if (NextTarget && NextTarget->EnemyTeamID == OwnerTeamID){
			FVector CreepLocation = NextTarget->GetRootComponent()->GetComponentLocation();
			float Distance = FVector::Dist(GetActorLocation(), CreepLocation);
			OnHitCreep(NextTarget, Distance);
		}
	}
}

void AAreaOfEffect::ActivateAtLocation(ABasicTower* SourceTower, FVector Location)
{
	TargetUnit = NULL;
	OwnerTower = SourceTower;
	if (OwnerTower) OwnerTeamID = OwnerTower->TeamID;
	else{
		Destroy();
		return;
	}
	SetActorLocation(Location);
	OnActivate(GetActorLocation());
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMulti(Overlaps, GetActorLocation(), FQuat::Identity,
		FCollisionShape::MakeSphere(Radius),
		FCollisionQueryParams(TEXT("TestThing"), false, this),
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllDynamicObjects));
	for (auto Overlap : Overlaps){
		ABasicCreep* NextTarget = Cast<ABasicCreep>(Overlap.GetActor());
		if (NextTarget && NextTarget->EnemyTeamID == OwnerTeamID){
			FVector CreepLocation = NextTarget->GetRootComponent()->GetComponentLocation();
			float Distance = FVector::Dist(GetActorLocation(), CreepLocation);
			OnHitCreep(NextTarget, Distance);
		}
	}
	if (Duration == 0){
		Destroy();
		return;
	}
	ElapsedTime = 0;
	NextProc = ProcRate;
	FinishTime = Duration;
	Active = true;
}

void AAreaOfEffect::ActivateAoE(ABasicTower* SourceTower, ABasicCreep* Target)
{
    TargetUnit = Target;
	OwnerTower = SourceTower;
    if (OwnerTower) OwnerTeamID = OwnerTower->TeamID;
	else{
        Destroy();
        return;
    }
	SetActorLocation(Target->GetRootComponent()->GetComponentLocation());
	OnActivate(GetActorLocation());
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMulti(Overlaps, GetActorLocation(), FQuat::Identity,
		FCollisionShape::MakeSphere(Radius),
		FCollisionQueryParams(TEXT("TestThing"), false, this),
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllDynamicObjects));
	for (auto Overlap : Overlaps){
		ABasicCreep* NextTarget = Cast<ABasicCreep>(Overlap.GetActor());
		if (NextTarget && NextTarget->EnemyTeamID == OwnerTeamID){
			FVector CreepLocation = NextTarget->GetRootComponent()->GetComponentLocation();
			float Distance = FVector::Dist(GetActorLocation(), CreepLocation);
			OnHitCreep(NextTarget, Distance);
		}
	}
    if (Duration == 0){
        Destroy();
        return;
    }
    ElapsedTime = 0;
    NextProc = ProcRate;
    FinishTime = Duration;
    Active = true;
}
