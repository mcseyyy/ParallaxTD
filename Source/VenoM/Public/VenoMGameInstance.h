// Copyright VenoM Inc. 2014-2015

#pragma once

#include "Engine/GameInstance.h"
#include "VenoMGameLevel.h"
#include "VenoMGameInstance.generated.h"

UCLASS()
class VENOM_API UVenoMGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
    FString LevelPath;
    FGameLevelData LevelData;
	int32 WinningTeam;
};
