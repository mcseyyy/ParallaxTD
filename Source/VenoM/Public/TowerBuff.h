// Copyright VenoM Inc. 2014-2015

#pragma once

#include "GameFramework/Actor.h"
#include "TowerBuff.generated.h"

class ABasicTower;

UCLASS()
class VENOM_API ATowerBuff : public AActor
{
	GENERATED_BODY()
public:	
	ATowerBuff();
	virtual void BeginPlay() override;
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	float Duration;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combat")
	float ProcRate;
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetRemainingTime(float RemainingTime);
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	float ElapsedTime;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	float NextProc;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	float FinishTime;
// Combat
public:
	void BuffTick(float DeltaSeconds);
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ProcBuff();
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	virtual void OnBuffStart();
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	virtual void BuffProc();
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	virtual void OnBuffEnd();
	UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	void RefreshBuff(ATowerBuff* NewBuff);
	virtual void RefreshBuff_Implementation(ATowerBuff* OldBuff);
	bool ActivateBuff(ABasicTower* TargetTower);
	void FinishBuff();
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Gameplay")
	bool Active;
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
	ABasicTower* Target;
};
