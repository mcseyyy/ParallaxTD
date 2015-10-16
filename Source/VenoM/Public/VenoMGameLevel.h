// Copyright VenoM Inc. 2014-2015

#pragma once

#include "Engine/LevelScriptActor.h"
#include "VenoMGameLevel.generated.h"

USTRUCT(BlueprintType)
struct FGameLevelData {
	GENERATED_USTRUCT_BODY()
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString Name;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 NumberOfTeams;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 PlayersPerTeam;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 StartingMoney;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 StartingIncome;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int32 StartingLives;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float WaveTime;
};

UCLASS()
class VENOM_API AVenoMGameLevel : public ALevelScriptActor
{
	GENERATED_BODY()
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game Data")
	FGameLevelData LevelData;
};
