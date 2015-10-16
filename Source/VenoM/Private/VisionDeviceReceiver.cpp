// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "VisionDeviceReceiver.h"
#include "NetworkHandler.h"

bool operator==(const TowerData& A, const TowerData& B){
    if (A.ID == B.ID) return true;
    return false;
};

FRect2D TowerCoordsToRect(TowerCoords Coords)
{
    FRect2D Rect;
    Rect.Centre = FVector2D(Coords.x, Coords.y);
    Rect.Size = FVector2D(1/16, 1/12);
    Rect.Rotation = (float)Coords.angle;
    return Rect;
}

VDRStatus AVisionDeviceReceiver::GetStatus()
{
    return Status;
}

VDRStatus AVisionDeviceReceiver::Update()
{
    switch (Status){
        case VDR_Offline:
            break;
		case VDR_Disconnected:
            if (NetworkHandler::acceptVision()){
                Status = VDR_Connected;
            }
            break;
        case VDR_Connected: {
            // Check for vision device disconnect
            // If vision device disconnected, set status to "WaitingForDevice"
            TArray<TowerCoords> NewTowers;
			TArray<TowerCoords> RemoveTowers;
			NetworkHandler::processVisionSocket(NewTowers, RemoveTowers);
            for (auto NewTower : NewTowers){
                AddNewTower(TowerCoordsToRect(NewTower));
			}
			for (auto Tower : RemoveTowers){
				RemoveTower(TowerCoordsToRect(Tower));
			}
            break;
        }
    }
    return Status;
}

void AVisionDeviceReceiver::SetupVisionServer()
{
    // Start listening for vision device connection
	Status = VDR_Disconnected;
    TowerCounter = 0;
}

void AVisionDeviceReceiver::ShutDownVisionServer()
{
	// Close the server and stop listening for vision device connection
	Status = VDR_Offline;
}

// Creates the list of coordinates for the player owned zones in both virtual
// space and meat-space, validating the input data as necessary.
// Takes an array of FRect2DPairs, with the first box representing the physical
// coordinates and the second representing the in-game coordinates.
void AVisionDeviceReceiver::SetPlayerZones(TArray<FCoordMap> ZoneCoords)
{
	TArray<TowerData> EmptyList;
    for (int32 ID = 0; ID < ZoneCoords.Num(); ++ID){
        if (!Rect2DContains(PhysicalSpaceBounds, (ZoneCoords[ID].Real))){
            DEBUG_MESSAGE(FString::Printf(TEXT("Zone %d outside of limits."), ID));
        }
        for (int32 NextID = ID + 1; NextID < ZoneCoords.Num(); ++NextID){
            if (Rect2DIntersect(ZoneCoords[ID].Real, ZoneCoords[NextID].Real)){
                DEBUG_MESSAGE(FString::Printf(
                    TEXT("Zones %d and %d clash physically."), ID, NextID));
            }
            if (Rect2DIntersect(ZoneCoords[ID].Game, ZoneCoords[NextID].Game)){
                DEBUG_MESSAGE(FString::Printf(
                    TEXT("Zones %d and %d clash in-game."), ID, NextID));
            }
        }
		TowerList.Push(EmptyList);
    }
    PlayerZones = ZoneCoords;
}

void AVisionDeviceReceiver::ResetTowerStatus(int32 PlayerID, int32 TowerID)
{
	for (auto Tower : TowerList[PlayerID]){
		if (Tower.ID == TowerID)
		{
			switch (Tower.Status){
				case TS_New:
					Tower.Status = TS_Normal;
					break;
				case TS_Removed:
					Tower.Status = TS_Gone;
					break;
				case TS_Restored:
					Tower.Status = TS_Normal;
					break;
				default:
					break;
			}
			return;
		}
	}
	DEBUG_MESSAGE("Failed to clear tower queue!");
}

// Returns a list of every tower owned by the given player
TArray<TowerData> AVisionDeviceReceiver::GetTowers(int32 PlayerID, bool ChangeStatus)
{
	TArray<TowerData> Results;
	if (PlayerID < 0 || PlayerID >= TowerList.Num()){
		DEBUG_MESSAGE(*FString::Printf(TEXT("%d, %d"), PlayerID, TowerList.Num()));
		return Results;
	}
    for (auto Tower : TowerList[PlayerID]){
        Results.Add(Tower);
        if (ChangeStatus){
            if (Tower.Status == TS_New) Tower.Status = TS_Normal;
			else if (Tower.Status == TS_Removed) Tower.Status = TS_Gone;
			else if (Tower.Status == TS_Restored) Tower.Status = TS_Normal;
        }
    }
    return Results;
}
// Returns a list of every tower owned by the given player with the given status
TArray<TowerData> AVisionDeviceReceiver::GetTowers(int32 PlayerID, TowerStatus TStatus, bool ChangeStatus)
{
    TArray<TowerData> Results;
	if (PlayerID < 0 || PlayerID >= TowerList.Num())
	{
		DEBUG_MESSAGE(*FString::Printf(TEXT("%d, %d"), PlayerID, TowerList.Num()));
		return Results;
	}
    for (auto& Tower : TowerList[PlayerID])
	{
        if (Tower.Status == TStatus)
		{
            Results.Add(Tower);
            if (ChangeStatus){
                if (Status == TS_New) Tower.Status = TS_Normal;
				else if (Tower.Status == TS_Removed) Tower.Status = TS_Gone;
				else if (Tower.Status == TS_Restored) Tower.Status = TS_Normal;
            }
        }
	}
    return Results;
}

// Returns the ID of the player who owns the zone in which TowerCoords is
// contained, or -1 if TowerCoords does not lie inside a zone
int32 AVisionDeviceReceiver::ZoneOwnerID(FRect2D PhysicalCoords)
{
    for (int32 ID = 0; ID < PlayerZones.Num(); ++ID){
        if (Rect2DContains(PlayerZones[ID].Real, PhysicalCoords)) return ID;
    }
    return -1;
}

void AVisionDeviceReceiver::AddNewTower(FRect2D PhysicalCoords)
{
    TowerData NewTower;
    int32 OwnerID = ZoneOwnerID(PhysicalCoords);
    if (OwnerID == -1) return;
    NewTower.GameCoords = RescaleRect(
        PlayerZones[OwnerID].Real, PlayerZones[OwnerID].Game, PhysicalCoords);
	for (auto Tower : TowerList[OwnerID]){
		if ((Tower.Status == TS_Gone || Tower.Status == TS_Removed) &&
			FVector2D::Distance(Tower.GameCoords.Centre, NewTower.GameCoords.Centre)
			<= (Tower.GameCoords.Size.X / 4)){
			Tower.Status = TS_Restored;
			return;
		}
	}
    NewTower.ID = TowerCounter++;
    NewTower.Status = TS_New;
    TowerList[OwnerID].Add(NewTower);
}
void AVisionDeviceReceiver::RemoveTower(FRect2D PhysicalCoords)
{
	TowerData OldTower;
	int32 OwnerID = ZoneOwnerID(PhysicalCoords);
	if (OwnerID == -1) return;
	OldTower.GameCoords = RescaleRect(
		PlayerZones[OwnerID].Real, PlayerZones[OwnerID].Game, PhysicalCoords);
	for (auto Tower : TowerList[OwnerID]){
		if (FVector2D::Distance(Tower.GameCoords.Centre, OldTower.GameCoords.Centre)
			<= (Tower.GameCoords.Size.X / 4)){
			Tower.Status = TS_Removed;
			return;
		}
	}
}

bool AVisionDeviceReceiver::PurgeTower(int32 TowerID)
{
	for (auto TowerArray : TowerList){
		for (auto Tower : TowerArray){
			if (Tower.ID == TowerID){
				TowerArray.RemoveSingle(Tower);
				return true;
			}
		}
	}
	return false;
}


void AVisionDeviceReceiver::SendStartToVision() //this starts the vision
{
	NetworkHandler::sendStartToVision();
}

void AVisionDeviceReceiver::SendResetToVision() //this stops the vision
{
	NetworkHandler::sendResetToVision();
}

void AVisionDeviceReceiver::SendCalibrateToVision() //this asks the vision to calibrate
{
	NetworkHandler::sendCalibrateToVision();
}

bool AVisionDeviceReceiver::ConfirmCalibration() //returns true when the vision is done calibrating
{
	return NetworkHandler::confirmCalibration();
}




