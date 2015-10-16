// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "BasicCreep.h"
#include "BasicTowerProjectile.generated.h"

class ABasicTower;

UCLASS()
class VENOM_API ABasicTowerProjectile : public AActor
{
	GENERATED_UCLASS_BODY()
// Engine
public:
	virtual void Tick(float DeltaSeconds) override;
// Movement
public:
    UFUNCTION(BlueprintImplementableEvent, Category = "Projectile Movement")
	virtual void OnFired(ABasicCreep* Target);
    UFUNCTION(BlueprintImplementableEvent, Category = "Projectile Movement")
    virtual void OnHitTarget();
    UFUNCTION(BlueprintNativeEvent, Category = "Projectile Movement")
    FVector ComputeVelocity(float DeltaSeconds);
    FVector Velocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Movement")
	float InitialSpeed = 1500.0f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Projectile Status")
    float TimeAlive;
// Collision
public:
    void SetupTargetParameters(FVector Location, float Radius, float HalfHeight);
    bool CollisionOnMove(const FVector& Movement, FVector& ImpactPoint);
    bool ChebyBounds(const FVector& Movement);
    bool BoxBoxSweep(const FVector& Movement);
    bool BoxPointSweep(const FVector& Movement);
    bool CapsuleCapsuleSweep(const FVector& Movement, FVector& ImpactPoint);
    bool CapsulePointSweep(const FVector& Movement, FVector& ImpactPoint);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Collision")
	USphereComponent* CollisionSphere;
// Target Tracking
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Movement")
	FVector TargetLocation;
    float TargetRadius;
    float TargetHalfHeight;
    FBox TargetBBox;
    float SelfRadius;
    FBox SelfBBox;
    float RadiusSum;
// Combat
public:
	UFUNCTION(BlueprintCallable, Category = "Projectile Combat")
	void FireProjectile(ABasicCreep *TargetEnemy, ABasicTower* SourceTower);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Combat")
	ABasicCreep* Target;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile Combat")
	ABasicTower* OwnerTower;
protected:
    void HitTargetEvent();
};
