// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "LobbyGameMode.h"


ALobbyGameMode::ALobbyGameMode(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	LoadingMap = false;
	timer = time(0);
}

void ALobbyGameMode::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList)
{
	Super::GetSeamlessTravelActorList(bToEntry, ActorList);
	if (PCH) ActorList.Add(PCH);
	if (VDR) ActorList.Add(VDR);
}

void ALobbyGameMode::PostSeamlessTravel()
{
    Super::PostSeamlessTravel();
    for (TActorIterator<APlayerConnectionHandler> PCHItr(GetWorld()); PCHItr; ++PCHItr){
        PCH = *PCHItr;
    }
    for (TActorIterator<AVisionDeviceReceiver> VDRItr(GetWorld()); VDRItr; ++VDRItr){
        VDR = *VDRItr;
    }
}

void ALobbyGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
    if (PCH){
		NetworkHandler::gameRunning = false; //I know, don't complain.
        PCH->Update();
        UpdatePlayers();
        for (int PlayerID = 0; PlayerID < MaxPlayers; PlayerID++){
            TArray<InputCommand> PlayerCommands = PCH->GetInput(PlayerID);
            for (auto Command : PlayerCommands){
                PlayerAction(PlayerID, Command.Command, Command.Operand);
            }
        }
		if (time(0) - timer > 1)
		{
			PCH->NotifyPlayerSlots(); // to check if clients are still connected and to update the position info on the client side.
			timer = time(0);
		}
    }
    TryStartGame();
}

void ALobbyGameMode::StartPlay()
{
	Super::StartPlay();
    auto GameInstance = Cast<UVenoMGameInstance>(GetWorld()->GetGameInstance());
    MaxTeams = GameInstance->LevelData.NumberOfTeams;
	MaxPlayers = MaxTeams * GameInstance->LevelData.PlayersPerTeam;
    if (!PCH){
	    FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = Instigator;
        PCH = GetWorld()->SpawnActor<APlayerConnectionHandler>(SpawnParams);
    }
	if (PCH->GetStatus() == PCH_Offline) PCH->SetupPlayerServer(MaxPlayers);
	UpdatePlayers();
    for (int32 c = 0; c < MaxPlayers; c++){
        ReadySet.Push(false);
        PendingSwaps.Push(-1);
    }
}

void ALobbyGameMode::DebugStart()
{
	/*UWorld* const World = GetWorld();
	if (World){
		FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = Instigator;
		if (!PCH) PCH = World->SpawnActor<APlayerConnectionHandler>(SpawnParams);
		if (!VDR) VDR = World->SpawnActor<AVisionDeviceReceiver>(SpawnParams);
        if (!PCH) DEBUG_MESSAGE("Failed to create PCH.");
        if (!VDR) DEBUG_MESSAGE("Failed to create VDR.");
	}
    else DEBUG_MESSAGE("World does not exist.");
	NetworkHandler::setUpServer(MaxPlayers);
	if (VDR->GetStatus() == VDR_Offline) VDR->SetupVisionServer();
	if (PCH->GetStatus() == PCH_Offline) PCH->SetupPlayerServer(MaxPlayers);
    auto GameInstance = Cast<UVenoMGameInstance>(World->GetGameInstance());
    GameInstance->PlayMap = FString("Example_Map");*/
}

bool ALobbyGameMode::ReadyToStart()
{
    for (auto Ready : ReadySet) if (!Ready) return false;
	return (VDR->GetStatus() == VDR_Connected);
}

void ALobbyGameMode::TryStartGame()
{
	if (ReadyToStart() && !LoadingMap){
        UWorld* const World = GetWorld();
		if (World){
			OnGameStart();
            for (int32 c = 0; c < MaxPlayers; c++) PCH->SendData(c, OutputData(SC_GameStarting, 0));
			auto GameInstance = Cast<UVenoMGameInstance>(World->GetGameInstance());
			LoadingMap = true;
			NetworkHandler::gameRunning = true;
			World->SeamlessTravel(GameInstance->LevelPath);
			VDR->SendStartToVision();
        }
    }
}

void ALobbyGameMode::SwapPlayers(int32 PlayerA, int32 PlayerB)
{
    OnPlayerSwap(PlayerA, PlayerB);
    if (Players[PlayerA].Status != EPlayerStatus::Disconnected){
		PCH->SendData(PlayerA, OutputData(SC_SwapSuccessful, PlayerB)); //NO NOTIFICATION SO FAR
    }
    if (Players[PlayerB].Status != EPlayerStatus::Disconnected){
        PCH->SendData(PlayerB, OutputData(SC_SwapSuccessful, PlayerA)); //NO NOTIFICATION SO FAR
    }
    PCH->SwapPlayerID(PlayerA, PlayerB);
    PendingSwaps[PlayerA] = -1;
    PendingSwaps[PlayerB] = -1;
    UpdatePlayers();
}

void ALobbyGameMode::CancelSwap(int32 PlayerID, int32 SwapTarget)
{
    if (PendingSwaps[PlayerID] != SwapTarget) return;
    PCH->SendData(PlayerID, OutputData(SC_SwapFailed, 0));
    PCH->SendData(SwapTarget, OutputData(SC_SwapFailed, 1));
    PendingSwaps[PlayerID] = -1;
    PendingSwaps[SwapTarget] = -1;
}

void ALobbyGameMode::PlayerAction(int32 PlayerID, receiveCommand Action, int32 Value)
{
    switch (Action){
        case RC_SetReady:
            OnPlayerSetReady(PlayerID, (Value != 0));
            ReadySet[PlayerID] = (Value != 0);
            break;
        case RC_MoveSlot:
            // TODO - Properly check Value and return appropriate error code
            if (Players[Value].Status == EPlayerStatus::Disconnected){
                SwapPlayers(PlayerID, Value);
            }
            else{
                if (PendingSwaps[Value] != -1){
                    PCH->SendData(PlayerID, OutputData(SC_SwapFailed, 0));
                }
                else{
                    PendingSwaps[PlayerID] = Value;
                    PendingSwaps[Value] = PlayerID;
                    PCH->SendData(Value, OutputData(SC_SwapRequested, PlayerID));
                }
            }
            break;
        case RC_AcceptSwap:
            if (PendingSwaps[PlayerID] != Value){
                PCH->SendData(PlayerID, OutputData(SC_SwapFailed, 0));
            }
            else{
                SwapPlayers(PlayerID, Value);
            }
            break;
        case RC_DeclineSwap:
            CancelSwap(PlayerID, Value);
            break;
        default:
            break;
    }
}

void ALobbyGameMode::UpdatePlayers()
{
    Players = PCH->GetPlayers(true);
    for (auto Player : Players){
        switch (Player.Status){
            case EPlayerStatus::Joined:
                Player.Status = EPlayerStatus::Connected;
                OnPlayerJoin(Player.ID);
                break;
            case EPlayerStatus::Left:
                OnPlayerLeave(Player.ID);
                Player.Status = EPlayerStatus::Disconnected;
                ReadySet[Player.ID] = false;
                PendingSwaps[Player.ID] = -1;
                break;
            default:
                break;
        }
    }
}
