// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Info.h"
#include "Debuff.generated.h"

class ABasicCreep;

UCLASS(Blueprintable)
class VENOM_API ADebuff : public AInfo
{
	GENERATED_BODY()
// Engine Functions
public:
	ADebuff(const FObjectInitializer& ObjectInitializer);
// Effects
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
    void DebuffTick(float DeltaSeconds);
    UFUNCTION(BlueprintCallable, Category = "Combat")
	void ProcDebuff();
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	virtual void OnDebuffStart();
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    virtual void DebuffProc();
    UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
    virtual void OnDebuffEnd();
    UFUNCTION(BlueprintNativeEvent, Category = "Combat")
    void RefreshDebuff(ADebuff* NewDebuff);
    virtual void RefreshDebuff_Implementation(ADebuff* OldDebuff);
    bool ActivateDebuff(ABasicCreep* Victim);
	void FinishDebuff();
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Gameplay")
	bool Active;
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Combat")
    ABasicCreep* Target;
};
