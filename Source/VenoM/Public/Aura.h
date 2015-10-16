// Copyright VenoM Inc. 2014-2015

#pragma once

#include "GameFramework/Actor.h"
#include "Aura.generated.h"

class ABasicCreep;
class ABasicTower;
class ABattlefield;

UCLASS()
class VENOM_API AAura : public AActor
{
	GENERATED_BODY()
		// Engine functions
public:
	AAura();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	// Effects
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	virtual void OnActivate(FVector Location);
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	virtual void OnHitTower(ABasicTower* Target, float Distance);
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	virtual void OnProcTower(ABasicTower* Target, float Distance);
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	float ProcRate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float Radius;
protected:
	void ProcTowers();
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	float NextProc;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	bool Active;
	// Combat
public:
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ActivateAura(ABasicTower* SourceTower);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ActivateAtLocation(ABasicTower* SourceTower, FVector Location);
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	ABasicTower* OwnerTower;
	int32 OwnerTeamID;
};
