// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "VenoM.h"

struct FVenoMGlobalStatus
{
    float CurrentTime;
    float NextWave;
    float WaveInterval;
    int32 WaveCount;
    TArray<int32> TeamSizes;
};
