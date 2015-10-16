// Copyright VenoM Inc. 2014-2015

#include "VenoM.h"
#include "PlayerConnectionHandler.h"
#include "VisionDeviceReceiver.h"
#include "PreLobbyGameMode.h"


void APreLobbyGameMode::StartPlay()
{
    Calibrated = false;
    if (VDR) VDR->SendCalibrateToVision();
    else Calibrated = true;
}

void APreLobbyGameMode::Tick(float DeltaSeconds)
{
    if (VDR){
        if (VDR->ConfirmCalibration()) Calibrated = true;
    }
    else{
        Calibrated = true;
        DEBUG_MESSAGE("Vision D/C!");
    }
    if (Calibrated) GoToLobby();
}

void APreLobbyGameMode::GoToLobby()
{
	UWorld* const World = GetWorld();
	if (World){
        World->SeamlessTravel(MAP_LOBBY);
	}
}

void APreLobbyGameMode::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList)
{
    Super::GetSeamlessTravelActorList(bToEntry, ActorList);
    if(PCH) ActorList.Add(PCH);
    if(VDR) ActorList.Add(VDR);
}

void APreLobbyGameMode::PostSeamlessTravel()
{
    Super::PostSeamlessTravel();
    for (TActorIterator<APlayerConnectionHandler> PCHItr(GetWorld()); PCHItr; ++PCHItr){
        PCH = *PCHItr;
    }
    for (TActorIterator<AVisionDeviceReceiver> VDRItr(GetWorld()); VDRItr; ++VDRItr){
        VDR = *VDRItr;
    }
}