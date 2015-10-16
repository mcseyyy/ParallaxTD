// Copyright VenoM Inc. 2014-2015

#pragma once

#include "Engine/TargetPoint.h"
#include "CreepMarker.generated.h"

UENUM(BlueprintType)
enum class ECreepSignal : uint8
{
        CS_Spawn    UMETA(DisplayName="Spawn"),
        CS_Goal     UMETA(DisplayName="Goal")
};

UCLASS()
class VENOM_API ACreepMarker : public ATargetPoint
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Enum)
    int32 TeamID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
	int32 SignalValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Enum)
	ECreepSignal CreepSignal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shinny)
	float MaterialPower = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shinny)
	float CoolDownTime = 15.f; //seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shinny)
	float PowerIncPerCreep = 0.05;

	void SpawnCreep();
};
