// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Info.h"
#include "NetworkHandler.h"
#include "PlayerConnectionHandler.generated.h"

enum PCHStatus {
	PCH_Offline,
	PCH_WaitingForPlayers,
	PCH_Full
};

UENUM(BlueprintType)
enum class EPlayerStatus : uint8
{
    Connected,
	Disconnected,
    Joined,
    Left
};

USTRUCT(BlueprintType)
struct FPlayerData {
	GENERATED_USTRUCT_BODY()
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	FString Name;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
	FColor Colour;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
    EPlayerStatus Status;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite)
    int32 ID;
};

 //Patrick said that these defines need some more brackets so I've put them there; 
#define GET_TEAM_ID(PlayerID)         ( (PlayerID) / MaxSlots)
#define GET_SLOT_ID(PlayerID)         ( (PlayerID) % MaxSlots)
#define GET_PLAYER_ID(TeamID,SlotID)  (((TeamID  ) * MaxSlots) + (SlotID))

UCLASS()
class VENOM_API APlayerConnectionHandler : public AInfo
{
    GENERATED_BODY()
    APlayerConnectionHandler(const FObjectInitializer& ObjectInitializer);
public:
	PCHStatus GetStatus();
    PCHStatus Update();
	void AddPlayer(int32 ClientID);
	void RemovePlayer(int32 ClientID);
	void SetupPlayerServer(int32 MaxClients);
	void ShutDownPlayerServer();
	void NotifyPlayerSlots();
	TArray<InputCommand> GetInput(int8 PlayerID);
	void SendData(int8 PlayerID, OutputData Output);
	void UpdatePlayerMoney(int8 PlayerID, int32 Money);
	void UpdatePlayerLives(int8 PlayerID, int32 Lives);
	void UpdatePlayerIncome(int8 PlayerID, int32 Income);
    void SwapPlayerID(int8 FirstID, int8 SecondID);
    TArray<FPlayerData> GetPlayers(bool ChangeState = false);
    TArray<FPlayerData> GetPlayers(EPlayerStatus Status, bool ChangeState = false);
protected:
	int32 GetClientID(int8 PlayerID);
	int8 FirstFreeID();
	TArray<FPlayerData> Clients;
	TArray<int8> ClientIDs;
	PCHStatus Status;
};
