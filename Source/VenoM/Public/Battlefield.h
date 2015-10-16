// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Info.h"
#include "BasicCreep.h"
#include "VenoMMath.h"
#include "BasicTower.h"
#include "CreepMarker.h"
#include "BasicTowerProjectile.h"
#include "irrKlang.h"
#include <array>
#include "Battlefield.generated.h"

DECLARE_DELEGATE_OneParam(TeamEventDelegate, int32);

UCLASS()
class VENOM_API ABattlefield : public AInfo
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
    void InitBattlefield(int32 TeamID, int32 StartingLives,
                         TArray<ACreepMarker*> CreepMarkers,
                         TSubclassOf<ACharacter> FakeCreepType,
                         irrklang::ISoundEngine* const Engine);
	UFUNCTION(BlueprintCallable, Category = "Units")
	bool SpawnCreep(TSubclassOf<ABasicCreep> CreepType, int32 ownerID);
	UFUNCTION(BlueprintCallable, Category = "Units")
	bool BuildTower(TSubclassOf<ABasicTower> TowerType, FRect2D TowerCoords, int32 PlayerID, int32 TowerID);
	UFUNCTION(BlueprintCallable, Category = "Units")
	APawn *SpawnPlaceholder(TSubclassOf<APawn> PlaceholderType, FRect2D Coords);
	void StartRemoveTower(int32 TowerID);
	void CancelRemoveTower(int32 TowerID);
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void SetLives(int32 NewLives);
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void InflictDamage(int32 DamageDealt);
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
    int32 GetLives();
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	float GetLivesPercentage();
    bool LivesChangedSinceLastCheck();
    TArray<ACharacter*> FakeCreeps;
	TArray<ABasicCreep*> Creeps;
	TArray<ABasicTower*> Towers;
	TArray<ACreepMarker*> CreepSpawnPoints;
	TArray<ACreepMarker*> CreepDestinationPoints;
    CreepTowerEventDelegate CreepKilledDelegate;
    CreepEventDelegate CreepFinishedDelegate;
	TeamEventDelegate TeamDefeatedDelegate;
	TowerEventDelegate TowerRemovedDelegate;
    void OnCreepKilled(ABasicCreep* Creep, ABasicTower* Tower);
	void OnCreepFinished(ABasicCreep* Creep);
	void OnTowerRemoved(ABasicTower* Tower);
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay")
    int32 TeamID;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Gameplay")
	int32 TargetTeamID;
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	bool IsActive();
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void KillTeam();
	void BigRed();
    bool BlocksPath(FRect2D Coords, TSubclassOf<AActor> FakeTowerType, TSubclassOf<APawn> FakeCreepType, int32 PlayerID);
	bool BlockedLocation(TSubclassOf<ABasicTower> TowerType, FRect2D Coords);
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void SetMusicTrack(int32 Track);
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    int32 GetNTracks();
protected:
    void TeamDefeated();
    
public:
	bool TeamActive;
    float CreepUpgradeTime;
    float TimeUntilUpgrade;
    float CreepUpgradeValue;
    float CreepMultiplierHP;
protected:
    void RemoveDeadCreeps();
    int32 Lives;
	int32 InitLives;
    bool LivesChanged;

// Audio
protected:
    void MusicInit(irrklang::ISoundEngine* const Engine);
    void MusicStop();
    void MusicTick(float DeltaSeconds);
    static const unsigned n_layers = 17;
    irrklang::ISoundEngine* Engine;
    std::array<irrklang::ISound*, n_layers> Layers;
    std::array<float, n_layers> TargetVolume;
    unsigned Configuration;
};
