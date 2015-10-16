// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#ifndef __VENOM_H__
#define __VENOM_H__

#include "Engine.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#define COLLISION_PROJECTILE ECC_GameTraceChannel1
#define COLLISION_CREEP      ECC_GameTraceChannel2

DECLARE_LOG_CATEGORY_EXTERN(LogVenoM, Log, All);

#define DEBUG_MESSAGE(x) GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("%s %d: "), "N", __LINE__) + (x))
//#define DEBUG_MESSAGE(x) (void)0

const int32 NUM_TEAMS   = 2;
const int32 NUM_PLAYERS = 3;

const FString MAP_MENU = FString("/Game/Maps/M_Menu");
const FString MAP_PRELOBBY = FString("/Game/Maps/M_PreLobby");
const FString MAP_LOBBY = FString("/Game/Maps/M_Lobby");
const FString MAP_EXAMPLE = FString("/Game/Maps/GameLevels/Example_Map");
const FString MAP_EXAMPLE2 = FString("/Game/Maps/GameLevels/Example_Map2");

#endif
