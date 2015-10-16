// Copyright VenoM Inc. 2014-2015

#include "VenoM.h"
#include "PlayGameMode.h"
#include "BasicTower.h"
#include "BasicCreep.h"
#include "Battlefield.h"
#include "Aura.h"


AAura::AAura()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AAura::BeginPlay()
{
	Super::BeginPlay();
	Active = false;
}

void AAura::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Active) return;
	if (OwnerTower && OwnerTower->IsPendingKill()){
		Destroy();
	};
	NextProc -= DeltaTime;
	if (NextProc <= 0){
		ProcTowers();
		NextProc += ProcRate;
	}
}

void AAura::ProcTowers()
{
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMulti(Overlaps, GetActorLocation(), FQuat::Identity,
		FCollisionShape::MakeSphere(Radius),
		FCollisionQueryParams(TEXT("TestThing"), false, this),
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllDynamicObjects));
	for (auto Overlap : Overlaps){
		ABasicTower* NextTarget = Cast<ABasicTower>(Overlap.GetActor());
		if (NextTarget && NextTarget->TeamID == OwnerTeamID){
			FVector TowerLocation = NextTarget->GetRootComponent()->GetComponentLocation();
			float Distance = FVector::Dist(GetActorLocation(), TowerLocation);
			OnProcTower(NextTarget, Distance);
		}
	}
}

void AAura::ActivateAtLocation(ABasicTower* SourceTower, FVector Location)
{
	OwnerTower = SourceTower;
	if (OwnerTower) OwnerTeamID = OwnerTower->TeamID;
	else{
		Destroy();
		return;
	}
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMulti(Overlaps, GetActorLocation(), FQuat::Identity,
		FCollisionShape::MakeSphere(Radius),
		FCollisionQueryParams(TEXT("TestThing"), false, this),
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllDynamicObjects));
	for (auto Overlap : Overlaps){
		ABasicTower* NextTarget = Cast<ABasicTower>(Overlap.GetActor());
		if (NextTarget && NextTarget->TeamID == OwnerTeamID){
			FVector TowerLocation = NextTarget->GetRootComponent()->GetComponentLocation();
			float Distance = FVector::Dist(GetActorLocation(), TowerLocation);
			OnHitTower(NextTarget, Distance);
		}
	}
	SetActorLocation(Location);
	OnActivate(GetActorLocation());
	NextProc = ProcRate;
	Active = true;
}

void AAura::ActivateAura(ABasicTower* SourceTower)
{
	OwnerTower = SourceTower;
	if (OwnerTower) OwnerTeamID = OwnerTower->TeamID;
	else{
		Destroy();
		return;
	}
	SetActorLocation(SourceTower->GetRootComponent()->GetComponentLocation());
	OnActivate(GetActorLocation());
	TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMulti(Overlaps, GetActorLocation(), FQuat::Identity,
		FCollisionShape::MakeSphere(Radius),
		FCollisionQueryParams(TEXT("TestThing"), false, this),
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllDynamicObjects));
	for (auto Overlap : Overlaps){
		ABasicTower* NextTarget = Cast<ABasicTower>(Overlap.GetActor());
		if (NextTarget && NextTarget->TeamID == OwnerTeamID){
			FVector TowerLocation = NextTarget->GetRootComponent()->GetComponentLocation();
			float Distance = FVector::Dist(GetActorLocation(), TowerLocation);
			OnHitTower(NextTarget, Distance);
		}
	}
	NextProc = ProcRate;
	Active = true;
}
