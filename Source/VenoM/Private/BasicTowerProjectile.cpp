// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "Engine.h"
#include "BasicTower.h"
#include "BasicTowerProjectile.h"

//Constructor
//Initialise the projectile's parameters
ABasicTowerProjectile::ABasicTowerProjectile(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	CollisionSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionSphere->InitSphereRadius(15.f);
	CollisionSphere->BodyInstance.SetCollisionProfileName("NoCollision");
	RootComponent = CollisionSphere;
	Target = NULL;
	PrimaryActorTick.bCanEverTick = true;
}

void ABasicTowerProjectile::HitTargetEvent()
{
    OnHitTarget();
    if (Target && Target->IsAlive()){
    	Target->InflictDamage(OwnerTower->Damage, OwnerTower);
    	if(OwnerTower) OwnerTower->OnHitCreep(Target);
    }
    Destroy();
}

// Tick
void ABasicTowerProjectile::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
    TimeAlive += DeltaSeconds;
	if (Target != NULL){
        if (!Target->IsAlive()){
            Target = NULL;
            RadiusSum = SelfRadius;
        }
        else{
            TargetLocation = Target->GetRootComponent()->GetComponentLocation();
            TargetBBox = TargetBBox.MoveTo(TargetLocation);
        }
	}
    FVector NextMove = ComputeVelocity(DeltaSeconds) * DeltaSeconds;
    FVector ImpactPoint;
    if (CollisionOnMove(NextMove, ImpactPoint)){
        SetActorLocation(ImpactPoint, false);
        HitTargetEvent();
    }
    else{
        SetActorLocation(RootComponent->GetComponentLocation() + NextMove, false);
        SelfBBox = SelfBBox.MoveTo(TargetLocation);
    }
}

FVector ABasicTowerProjectile::ComputeVelocity_Implementation(float DeltaSeconds)
{
    return (TargetLocation - RootComponent->GetComponentLocation()).GetSafeNormal() * InitialSpeed;
}

// Collision detection function that calls other collision detection functions
// of increasing complexity. Returns true iff the projectile would collide with
// the target during the given move.
// ImpactPoint is set to a suitable point for the projectile to collide at if a
// collision is found during the move.
bool ABasicTowerProjectile::CollisionOnMove(const FVector& Movement, FVector& ImpactPoint)
{
    if (!ChebyBounds(Movement)) return false;
    if (Target){
        if (!BoxBoxSweep(Movement)) return false;
        if (!CapsuleCapsuleSweep(Movement, ImpactPoint)) return false;
    }
    else{
        if (!BoxPointSweep(Movement)) return false;
        if (!CapsulePointSweep(Movement, ImpactPoint)) return false;
    }
    return true;
}

// Rough collision detection, returns false iff the projectile is too far away
// from the target to possibly reach it in one move at its current velocity.
bool ABasicTowerProjectile::ChebyBounds(const FVector& Movement)
{
    FVector ChebyDiff = TargetLocation - RootComponent->GetComponentLocation();
    float ChebyDist = ChebyDiff.GetAbsMax() - RadiusSum;
    if (ChebyDist <= Movement.GetAbsMax()) return true;
    return false;
}

// Slightly more refined collision detection, returns false iff the projectile
// is continuously non-overlapping with the target on at least 1 axis.
bool ABasicTowerProjectile::BoxBoxSweep(const FVector& Movement)
{
    FBox SweepBBox = FBox(SelfBBox);
    if (Movement.X < 0) SweepBBox.Min.X += Movement.X;
    else SweepBBox.Max.X += Movement.X;
    if (Movement.Y < 0) SweepBBox.Min.Y += Movement.Y;
    else SweepBBox.Max.Y += Movement.Y;
    if (Movement.Z < 0) SweepBBox.Min.Z += Movement.Z;
    else SweepBBox.Max.Z += Movement.Z;
    if (SweepBBox.Intersect(TargetBBox)) return true;
    if (SweepBBox.IsInside(TargetBBox)) return true;
    return false;
}
bool ABasicTowerProjectile::BoxPointSweep(const FVector& Movement)
{
    FBox SweepBBox = FBox(SelfBBox);
    if (Movement.X < 0) SweepBBox.Min.X += Movement.X;
    else SweepBBox.Max.X += Movement.X;
    if (Movement.Y < 0) SweepBBox.Min.Y += Movement.Y;
    else SweepBBox.Max.Y += Movement.Y;
    if (Movement.Z < 0) SweepBBox.Min.Z += Movement.Z;
    else SweepBBox.Max.Z += Movement.Z;
    if (SweepBBox.IsInside(TargetLocation)) return true;
    return false;
}

// Fully accurate collision detection, returns false iff the projectile does
// not overlap the target at any point during its move.
// ImpactPoint is set to a suitable point for the projectile to collide at if a
// collision is found during the move.
bool ABasicTowerProjectile::CapsuleCapsuleSweep(const FVector& Movement, FVector& ImpactPoint)
{
    FVector TargetA = TargetLocation - TargetHalfHeight;
    FVector TargetB = TargetLocation + TargetHalfHeight;
    FVector SelfA = RootComponent->GetComponentLocation();
    FVector SelfB = SelfA + Movement;
    FVector InnerPoint;
    FMath::SegmentDistToSegment(TargetA, TargetB, SelfA, SelfB, InnerPoint, ImpactPoint);
    float Distance = FVector::Dist(InnerPoint, ImpactPoint);
    if (Distance <= TargetRadius + SelfRadius) return true;
    return false;
}
bool ABasicTowerProjectile::CapsulePointSweep(const FVector& Movement, FVector& ImpactPoint)
{
    FVector SelfA = RootComponent->GetComponentLocation();
    FVector SelfB = SelfA + Movement;
    ImpactPoint = FMath::ClosestPointOnSegment(TargetLocation, SelfA, SelfB);
    float Distance = FVector::Dist(TargetLocation, ImpactPoint);
    if (Distance <= SelfRadius) return true;
    return false;
}

void ABasicTowerProjectile::SetupTargetParameters(FVector Location, float Radius, float HalfHeight)
{
    TargetLocation = Location;
    TargetRadius = Radius;
    TargetHalfHeight = HalfHeight;
    FVector TargetExtents = FVector(TargetRadius, TargetRadius, TargetRadius + TargetHalfHeight);
    TargetBBox = FBox(TargetLocation - TargetExtents, TargetLocation + TargetExtents);
    RadiusSum = TargetRadius + TargetHalfHeight + SelfRadius;
}

//Fire Projectile
//It sets the projectile's damage, target and debuff
void ABasicTowerProjectile::FireProjectile(ABasicCreep* TargetEnemy, ABasicTower* FiringTower)
{
	TimeAlive = 0.f;
	Target = TargetEnemy;
    OwnerTower = FiringTower;
    SelfRadius = CollisionSphere->GetScaledSphereRadius();
    SelfBBox = FBox(RootComponent->GetComponentLocation() - SelfRadius,
                    RootComponent->GetComponentLocation() + SelfRadius);
    SetupTargetParameters(Target->GetRootComponent()->GetComponentLocation(),
                          Target->GetCapsuleComponent()->GetScaledCapsuleRadius(),
                          Target->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	SetActorTickEnabled(true);
	OnFired(Target);
}
