// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "GameFramework/Info.h"
#include "PlayerConnectionHandler.h"
#include "VenoMMath.h"
#include "VisionDeviceReceiver.generated.h"

enum VDRStatus {
	VDR_Offline,
	VDR_Disconnected,
	VDR_Connected
};

enum TowerStatus {
	TS_Normal,
    TS_New,
	TS_Removed,
	TS_Gone,
	TS_Restored,
	TS_Invalid,
	TS_Queued, //For when the player does not have enough money
	TS_Blocked //Tower blocks the path
};

struct TowerData {
    FRect2D GameCoords;
    int32 ID;
    TowerStatus Status;
	APawn *Placeholder;
};

struct FCoordMap {
    FRect2D Real;
    FRect2D Game;
};

bool operator==(const TowerData& A, const TowerData& B);

const FRect2D TEAM_BOUNDS_2[2] = {
        { FVector2D(0.25, 0.5), FVector2D(0.5, 1.f), 0.f   },
        { FVector2D(0.75, 0.5), FVector2D(0.5, 1.f), 180.f }
};

const FRect2D PhysicalSpaceBounds = { FVector2D(0.5, 0.5), FVector2D(1, 1), 0.f };

FRect2D TowerCoordsToRect(TowerCoords Coords);

UCLASS()
class VENOM_API AVisionDeviceReceiver : public AInfo
{
    GENERATED_BODY()
public:
    VDRStatus GetStatus();
    VDRStatus Update();
	void SetupVisionServer();
	void ShutDownVisionServer();
	void SetPlayerZones(TArray<FCoordMap> Coords);
	void ResetTowerStatus(int32 PlayerID, int32 TowerID);
    TArray<TowerData> GetTowers(int32 PlayerID, bool ChangeStatus = false);
    TArray<TowerData> GetTowers(int32 PlayerID, TowerStatus TStatus, bool ChangeStatus = false);
    TArray<TowerData> GetUnbuiltTowers();
	void FinishBuildingTower(int32 TowerID);
	bool PurgeTower(int32 TowerID);
	void SendStartToVision();
	void SendResetToVision();
	void SendCalibrateToVision();
	bool ConfirmCalibration();
private:
    int32 ZoneOwnerID(FRect2D PhysicalCoords);
	void AddNewTower(FRect2D PhysicalCoords);
	void RemoveTower(FRect2D PhysicalCoords);
    VDRStatus Status;
public:
    TArray<TArray<TowerData>> TowerList;
private:
    TArray<TowerData> UnbuiltTowerList;
	TArray<FCoordMap> PlayerZones;
    FVector2D TowerSize;
    int32 TowerCounter;
};
