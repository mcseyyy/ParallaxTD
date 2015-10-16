// Copyright VenoM Inc. 2014-2015

#pragma once

#include "GameFramework/GameMode.h"
#include "PreLobbyGameMode.generated.h"

class APlayerConnectionHandler;
class AVisionDeviceReceiver;

UCLASS()
class VENOM_API APreLobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	virtual void StartPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList) override;
	virtual void PostSeamlessTravel() override;
public:
    void GoToLobby();
protected:
    APlayerConnectionHandler* PCH;
    AVisionDeviceReceiver* VDR;
    bool Calibrated;
};
