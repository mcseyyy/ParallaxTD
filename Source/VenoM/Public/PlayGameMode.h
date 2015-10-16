// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "NetworkHandler.h"
#include "VenoMMath.h"
#include "irrKlang.h"
#include "PlayGameMode.generated.h"

class APlayerStats;
class ABattlefield;
class APlayerConnectionHandler;
class AVisionDeviceReceiver;
class ACreepMarker;
class APlayerZone;
class AVenoMGameInstance;

UCLASS()
class VENOM_API APlayGameMode : public AGameMode
{
	GENERATED_BODY()
// Engine functions
public:
	APlayGameMode(const FObjectInitializer& ObjectInitializer);
	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) override;
	virtual void PostSeamlessTravel() override;
// Game management
public:
	UFUNCTION(BlueprintCallable, Category = "Game Overview")
	void RemoveSplitscreen();
	UFUNCTION(BlueprintCallable, Category = "Game Overview")
	void ReturnToMenu();
	UFUNCTION(BlueprintCallable, Category = "Game Overview")
    void ReturnToLobby();
    UFUNCTION(BlueprintCallable, Category = "Debugging")
    void DebugStart();
    irrklang::ISoundEngine* Engine;
// Gameplay
public:
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    ABattlefield* GetTeam(int32 TeamID);
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    APlayerStats* GetPlayer(int32 PlayerID);
    UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	virtual void OnCreepKilled(ABasicCreep* Creep, ABasicTower* Tower);
    UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
	virtual void OnCreepFinished(ABasicCreep* Creep);
    UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
    virtual void OnTeamDefeated(int32 TeamID);
    UFUNCTION(BlueprintImplementableEvent, Category = "Gameplay")
    virtual void OnTeamVictory(int32 TeamID);
    void CreepKilledEvent(ABasicCreep* Creep, ABasicTower* Tower);
    void CreepFinishedEvent(ABasicCreep* Creep);
    void TeamDefeatedEvent(int32 TeamID);
	void TeamVictoryEvent(int32 TeamID);
	void TowerRemovedEvent(ABasicTower* Tower);
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay")
    bool GameActive;
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay")
    float GameTime;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay")
    float WaveTime;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay")
    float NextWave;
protected:
	void AddCreepBP(TCHAR *path);
	void AddTowerBP(TCHAR *path);
    void PlayerGameAction(int32 PlayerID, receiveCommand Action, int32 Value);
    void PlayerPostGameAction(int32 PlayerID, receiveCommand Action, int32 Value);
    void InitPlayerZones();
    void InitTeams();
    
    void HandleClientInput();
    void HandleVisionInput();
    APlayerConnectionHandler* PCH;
    AVisionDeviceReceiver* VDR;
    TArray<ABattlefield*> Teams;
    TArray<APlayerStats*> Players;
    TArray<TSubclassOf<ABasicCreep>> CreepTypes;
    TArray<TSubclassOf<ABasicTower>> TowerTypes;
    TSubclassOf<ACharacter> FakeCreepType;
    TSubclassOf<AActor> FakeTowerType;
	TSubclassOf<APawn> PlaceHolder_NoMoney;
	TSubclassOf<APawn> PlaceHolder_NoPath;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay")
    int32 MaxPlayers;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay")
    int32 MaxTeams;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay")
    int32 MaxSlots;
};
