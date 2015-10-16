// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "PlayerConnectionHandler.h"
#include "NetworkHandler.h"

const int32 MaxPlayerDefaults = 6;
const FColor PlayerDefaultColours[MaxPlayerDefaults] = {
    FColor::FromHex(FString("#494ff7")), // Blue
    FColor::FromHex(FString("#bf3beb")), // Purple
    FColor::FromHex(FString("#25c36a")),  // Green
    FColor::FromHex(FString("#ff0101")), // Red
    FColor::FromHex(FString("#f7941d")), // Orange
    FColor::FromHex(FString("#fee101"))  // Yellow
};
const FString PlayerDefaultNames[MaxPlayerDefaults] = {
    FString("AAA"),
    FString("BBB"),
    FString("CCC"),
    FString("DDD"),
    FString("EEE"),
    FString("FFF")
};

APlayerConnectionHandler::APlayerConnectionHandler(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Status = PCH_Offline;
}

PCHStatus APlayerConnectionHandler::GetStatus()
{
    return Status;
}

PCHStatus APlayerConnectionHandler::Update()
{
    switch (Status){
        case PCH_Offline:
            break;
        case PCH_WaitingForPlayers:
        {
            int32 ClientID;
            while (NetworkHandler::acceptClient(ClientID) && Status != PCH_Full) AddPlayer(ClientID);
            break;
        }
		case PCH_Full:
			int32 ClientID;
			NetworkHandler::acceptClient(ClientID);
            // Check for player disconnect
            // If vision device disconnected, set status to "WaitingForDevice"

			//this just makes sure players who DCed can reconnect even when the lobby is full.
            break;
    }
    return Status;
}

void APlayerConnectionHandler::AddPlayer(int32 ClientID)
{
    int8 PlayerID = FirstFreeID();
    Clients[ClientID].Status = EPlayerStatus::Joined;
    Clients[ClientID].ID = PlayerID;
    ClientIDs[PlayerID] = ClientID;
    if (ClientID >= 0 && ClientID < MaxPlayerDefaults){
		Clients[ClientID].Name = "";
		for (int32 c = 0; c < 3; c++) Clients[ClientID].Name += NetworkHandler::names[ClientID][c];
        Clients[ClientID].Colour = PlayerDefaultColours[ClientID];
    }
    //NOTIFY ALL PLAYERS (including the new player) ABOUT THE NEW PLAYER HERE
	NotifyPlayerSlots();
    if (FirstFreeID() == -1) Status = PCH_Full;
}

void APlayerConnectionHandler::RemovePlayer(int32 ClientID)
{
    //Clients[ClientID].Status = EPlayerStatus::Left;
}

void APlayerConnectionHandler::SetupPlayerServer(int32 MaxClients)
{
    Status = PCH_WaitingForPlayers;
    FPlayerData BlankPlayer;
    BlankPlayer.ID = -1;
    BlankPlayer.Status = EPlayerStatus::Disconnected;
    BlankPlayer.Colour = FColor::FromHex(FString("#808080"));
    BlankPlayer.Name = FString("Anonymous");
    Clients.Init(BlankPlayer, MaxClients);
    ClientIDs.Init(-1, MaxClients);
}

void APlayerConnectionHandler::ShutDownPlayerServer()
{
	// Do some magic
	Status = PCH_Offline;
}

void APlayerConnectionHandler::NotifyPlayerSlots()
{
	NetworkHandler::notifyPlayerSlots();
}

TArray<InputCommand> APlayerConnectionHandler::GetInput(int8 PlayerID)
{
    int32 ClientID = GetClientID(PlayerID);
    TArray<InputCommand> PlayerCommands;
    if (ClientID == -1 || Clients[ClientID].Status == EPlayerStatus::Disconnected){
        return PlayerCommands;
    }
    NetworkHandler::processClientSocket(ClientID, PlayerCommands);
    return PlayerCommands;
}

void APlayerConnectionHandler::SendData(int8 PlayerID, OutputData Output)
{
	int32 ClientID = GetClientID(PlayerID);
	NetworkHandler::sendCommandToClient(ClientID, Output);
}

void APlayerConnectionHandler::UpdatePlayerMoney(int8 PlayerID, int32 Money)
{
	int32 ClientID = GetClientID(PlayerID);
	SendData(ClientID, OutputData(SC_AmountOfMoney, Money));
}

void APlayerConnectionHandler::UpdatePlayerLives(int8 PlayerID, int32 Lives)
{
	int32 ClientID = GetClientID(PlayerID);
	SendData(ClientID, OutputData(SC_Lives, Lives));
}

void APlayerConnectionHandler::UpdatePlayerIncome(int8 PlayerID, int32 Income)
{
	int32 ClientID = GetClientID(PlayerID);
	SendData(ClientID, OutputData(SC_Income, Income));
}

// Swaps the PlayerIDs of the two players with the given IDs. If one of the
// players does not exist, that player is unaffected.
void APlayerConnectionHandler::SwapPlayerID(int8 FirstID, int8 SecondID)
{
    int32 FirstClient = GetClientID(FirstID);
    int32 SecondClient = GetClientID(SecondID);
    if (FirstClient >= 0) Clients[FirstClient].ID = SecondID;
    if (SecondClient >= 0) Clients[SecondClient].ID = FirstID;
	char tempName[3];
	tempName[0] = NetworkHandler::names[FirstID][0];
	tempName[1] = NetworkHandler::names[FirstID][1];
	tempName[2] = NetworkHandler::names[FirstID][2];

	NetworkHandler::names[FirstID][0] = NetworkHandler::names[SecondID][0];
	NetworkHandler::names[FirstID][1] = NetworkHandler::names[SecondID][1];
	NetworkHandler::names[FirstID][2] = NetworkHandler::names[SecondID][2];

	NetworkHandler::names[SecondID][0] = tempName[0];
	NetworkHandler::names[SecondID][1] = tempName[1];
	NetworkHandler::names[SecondID][2] = tempName[2];

	NotifyPlayerSlots();
}

TArray<FPlayerData> APlayerConnectionHandler::GetPlayers(bool ChangeState)
{
    TArray<FPlayerData> Results;
    for (auto Client : Clients){
        Results.Add(Client);
        if (ChangeState){
            switch (Client.Status){
                case EPlayerStatus::Joined:
                    Client.Status = EPlayerStatus::Connected;
                    break;
                case EPlayerStatus::Left:
                    Client.Status = EPlayerStatus::Disconnected;
                    break;
                default:
                    break;
            }
        }
    }
    return Results;
}

TArray<FPlayerData> APlayerConnectionHandler::GetPlayers(EPlayerStatus Status, bool ChangeState)
{
    TArray<FPlayerData> Results;
    for (auto Client : Clients){
        if (Client.Status == Status)
        {
            Results.Add(Client);
            if (ChangeState){
                switch (Status){
                    case EPlayerStatus::Joined:
                        Client.Status = EPlayerStatus::Connected;
                        break;
                    case EPlayerStatus::Left:
                        Client.Status = EPlayerStatus::Disconnected;
                        break;
                    default:
                        break;
                }
            }
        }
    }
    return Results;
}

int32 APlayerConnectionHandler::GetClientID(int8 PlayerID)
{
	for (int32 C = 0; C < Clients.Num(); C++){
		if (Clients[C].ID == PlayerID) return C;
	}
	return -1;
}

int8 APlayerConnectionHandler::FirstFreeID()
{
    TArray<bool> Available;
    Available.Init(true, Clients.Num());
    for (auto Client : Clients){
        if(Client.Status != EPlayerStatus::Disconnected) Available[Client.ID] = false;
    }
    for (int8 Result = 0; Result < Clients.Num(); Result++){
        if (Available[Result]) return Result;
    }
	return -1;
}
