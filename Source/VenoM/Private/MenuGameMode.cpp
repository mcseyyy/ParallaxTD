// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "MenuGameMode.h"
#include "Overseer.h"
#include "AssetRegistryModule.h"

AMenuGameMode::AMenuGameMode(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    WaitingToStart = false;
    DefaultPawnClass = AOverseer::StaticClass();
}

void AMenuGameMode::LoadMaps(FString LevelDirectory)
{
    FGameLevelData ExampleMapData;
	ExampleMapData.Name = FString(TEXT("ExampleMap"));
	ExampleMapData.NumberOfTeams = 2;
	ExampleMapData.PlayersPerTeam = 3;
	ExampleMapData.StartingMoney = 750;
	ExampleMapData.StartingIncome = 75;
	ExampleMapData.StartingLives = 150;
	ExampleMapData.WaveTime = 5;
    LevelList.Add(ExampleMapData);
	LevelPaths.Add(MAP_EXAMPLE);
	ExampleMapData.Name = FString(TEXT("ExampleMap2"));
	ExampleMapData.NumberOfTeams = 2;
	ExampleMapData.PlayersPerTeam = 3;
	ExampleMapData.StartingMoney = 750;
	ExampleMapData.StartingIncome = 75;
	ExampleMapData.StartingLives = 150;
	ExampleMapData.WaveTime = 5;
	LevelList.Add(ExampleMapData);
	LevelPaths.Add(MAP_EXAMPLE2);
}

void AMenuGameMode::StartPlay()
{
	Super::StartPlay();
	if (NetworkHandler::serverRunning) NetworkHandler::stopServer();
	UWorld* const World = GetWorld();
	if (World){
		FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = Instigator;
		PCH = World->SpawnActor<APlayerConnectionHandler>(SpawnParams);
		VDR = World->SpawnActor<AVisionDeviceReceiver>(SpawnParams);
        if (!PCH) DEBUG_MESSAGE("Failed to create PCH.");
        if (!VDR) DEBUG_MESSAGE("Failed to create VDR.");
        LoadMaps(FString(TEXT("/Game/Maps/GameLevels")));
	}
    else DEBUG_MESSAGE("World does not exist.");
}

void AMenuGameMode::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
	if (PCH) ActorList.Add(PCH);
	if (VDR) ActorList.Add(VDR);
}

void AMenuGameMode::PostSeamlessTravel()
{
    Super::PostSeamlessTravel();
    for (TActorIterator<APlayerConnectionHandler> PCHItr(GetWorld()); PCHItr; ++PCHItr){
        (*PCHItr)->Destroy();
    }
    for (TActorIterator<AVisionDeviceReceiver> VDRItr(GetWorld()); VDRItr; ++VDRItr){
        (*VDRItr)->Destroy();
    }
}

void AMenuGameMode::TryStartLobby(FString LevelName)
{
	WaitingToStart = true;
	UWorld* const World = GetWorld();
    if (World){
        int c = 0;
        while (c < LevelList.Num() && LevelList[c].Name != LevelName){
            c++;
        }
        if (c != LevelList.Num()){
            auto GameInstance = Cast<UVenoMGameInstance>(World->GetGameInstance());
            if (GameInstance){
                GameInstance->LevelPath = LevelPaths[c];
				GameInstance->LevelData = LevelList[c];
				if (NetworkHandler::serverRunning) NetworkHandler::stopServer();
				NetworkHandler::setUpServer(LevelList[c].NumberOfTeams*LevelList[c].PlayersPerTeam);
				VDR->SetupVisionServer();
            }
            else{
                DEBUG_MESSAGE("GameInstance is not of correct type.");
                CancelStartLobby();
            }
        }
        else{
            DEBUG_MESSAGE(FString::Printf(TEXT("%s is not a valid level."), *LevelName));
            CancelStartLobby();
        }
    }
}

void AMenuGameMode::CancelStartLobby()
{
    WaitingToStart = false;
	VDR->ShutDownVisionServer();
}

void AMenuGameMode::StartTheLobby()
{
	UWorld* const World = GetWorld();
	if (World){
        World->SeamlessTravel(MAP_PRELOBBY);
	}
    else{
        DEBUG_MESSAGE("No world found.");
        CancelStartLobby();
    }
}

void AMenuGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (WaitingToStart){
        if (VDR->Update() == VDR_Connected){
            StartTheLobby();
        }
    }
}