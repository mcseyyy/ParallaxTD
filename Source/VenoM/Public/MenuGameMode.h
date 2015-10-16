// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "PlayerConnectionHandler.h"
#include "VisionDeviceReceiver.h"
#include "VenoMGameInstance.h"
#include "MenuGameMode.generated.h"

UCLASS()
class VENOM_API AMenuGameMode : public AGameMode
{
	GENERATED_BODY()
    AMenuGameMode(const FObjectInitializer& ObjectInitializer);
public:
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) override;
    virtual void PostSeamlessTravel() override;
	virtual void StartPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    UFUNCTION(BlueprintCallable, Category = Buttons)
    void TryStartLobby(FString LevelName);
    UFUNCTION(BlueprintCallable, Category = Buttons)
    void CancelStartLobby();
    APlayerConnectionHandler* PCH;
    AVisionDeviceReceiver* VDR;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Game Data")
    TArray<FGameLevelData> LevelList;
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Game Data")
    TArray<FString> LevelPaths;
private:
    void LoadMaps(FString LevelDirectory);
    void StartTheLobby();
    bool WaitingToStart;
};
