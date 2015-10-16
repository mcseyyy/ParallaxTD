// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "PlayerConnectionHandler.h"
#include "VisionDeviceReceiver.h"
#include "VenoMGameInstance.h"
#include <time.h>
#include "LobbyGameMode.generated.h"

UCLASS()
class VENOM_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ALobbyGameMode(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaSeconds) override;
	virtual void StartPlay() override;
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) override;
    virtual void PostSeamlessTravel() override;
    UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
    void OnPlayerJoin(int32 PlayerID);
    UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
    void OnPlayerLeave(int32 PlayerID);
    UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
    void OnPlayerSwap(int32 PlayerA, int32 PlayerB);
    UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
	void OnPlayerSetReady(int32 PlayerID, bool Ready);
	UFUNCTION(BlueprintImplementableEvent, Category = "Lobby")
	void OnGameStart();
	bool ReadyToStart();
	void TryStartGame();
    UFUNCTION(BlueprintCallable, Category = "Debugging")
    void DebugStart();
protected:
    void PlayerAction(int32 PlayerID, receiveCommand Action, int32 Value);
	void UpdatePlayers();
    void SwapPlayers(int32 PlayerA, int32 PlayerB);
    void CancelSwap(int32 PlayerID, int32 SwapTarget);
    int32 MaxPlayers;
    int32 MaxTeams;
    APlayerConnectionHandler* PCH;
    AVisionDeviceReceiver* VDR;
    TArray<int32> PendingSwaps;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Lobby")
    TArray<bool> ReadySet;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Lobby")
    TArray<FPlayerData> Players;
	bool LoadingMap;
	time_t timer;
};
