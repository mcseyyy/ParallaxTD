// Copyright VenoM Inc. 2014-2015

#pragma once

#include "GameFramework/Actor.h"
#include "AreaOfEffect.generated.h"

class ABasicCreep;
class ABasicTower;
class ABattlefield;

UCLASS()
class VENOM_API AAreaOfEffect : public AActor
{
	GENERATED_BODY()
// Engine functions
public:	
	AAreaOfEffect();
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
// Effects
public:	
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    virtual void OnActivate(FVector Location);
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    virtual void OnHitCreep(ABasicCreep* Target, float Distance);
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    virtual void OnProcCreep(ABasicCreep* Target, float Distance);
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
    float Duration;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
    float ProcRate;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float Radius;
protected:
    void ProcCreeps();
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
    bool Active;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
    float ElapsedTime;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
    float NextProc;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
    float FinishTime;
// Combat
public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ActivateAoE(ABasicTower* SourceTower, ABasicCreep* Target);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ActivateAtLocation(ABasicTower* SourceTower, FVector Location);
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	ABasicCreep* TargetUnit;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	ABasicTower* OwnerTower;
	int32 OwnerTeamID;
};
