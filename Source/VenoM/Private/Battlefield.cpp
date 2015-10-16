// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "Battlefield.h"

#include "AIController.h"
#include "PlayGameMode.h"


ABattlefield::ABattlefield(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;
    SetActorTickEnabled(true);
    CreepUpgradeTime = 60.0;
    CreepMultiplierHP = 1.0;
    CreepUpgradeValue = 0.3;
}

//Tick
//Find targets for all towers
//Attack the targets
//Release dead/gone targets
//Remove dead creeps
void ABattlefield::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
    RemoveDeadCreeps();
    TimeUntilUpgrade -= DeltaSeconds;
    if (TimeUntilUpgrade <= 0){
        TimeUntilUpgrade += CreepUpgradeTime;
        CreepMultiplierHP += CreepUpgradeValue;
    }

	if (TeamActive)
		MusicTick(DeltaSeconds);
}

void ABattlefield::BeginPlay()
{
	Super::BeginPlay();
    TimeUntilUpgrade = CreepUpgradeTime;
}

void ABattlefield::InitBattlefield(int32 TeamID, int32 StartingLives,
                                   TArray<ACreepMarker*> CreepMarkers,
                                   TSubclassOf<ACharacter> FakeCreepType,
                                   irrklang::ISoundEngine* const Engine)
{
    this->TeamID = TeamID;
	InitLives = StartingLives;
    SetLives(StartingLives);
    for (auto CreepMarker : CreepMarkers){
        switch (CreepMarker->CreepSignal){
            case ECreepSignal::CS_Spawn:
                CreepSpawnPoints.Add(CreepMarker);
                break;
            case ECreepSignal::CS_Goal:
                CreepDestinationPoints.Add(CreepMarker);
                break;
        }
    }
    CreepSpawnPoints.Sort(
        [](const ACreepMarker& A, const ACreepMarker& B){
            return (A.SignalValue > B.SignalValue);
        });
    CreepDestinationPoints.Sort(
        [](const ACreepMarker& A, const ACreepMarker& B){
            return (A.SignalValue > B.SignalValue);
        });
    if (CreepSpawnPoints.Num() == 0) DEBUG_MESSAGE(TEXT("Team missing spawn points."));
    if (CreepDestinationPoints.Num() == 0) DEBUG_MESSAGE(TEXT("Team missing destination points."));
    TeamActive = true;
    MusicInit(Engine);
}

//Remove Dead Creeps
//Take a guess.
void ABattlefield::RemoveDeadCreeps()
{
    Creeps.RemoveAll(
        [](ABasicCreep* Creep){
            return Creep == NULL || Creep->IsPendingKill();
        });
}

void ABattlefield::SetLives(int32 NewLives)
{
    if (NewLives < 0) NewLives = 0;
    if (Lives != NewLives){
        Lives = NewLives;
        LivesChanged = true;
        if (Lives == 0) TeamDefeated();
    }
}

void ABattlefield::InflictDamage(int32 DamageDealt)
{
    SetLives(Lives - DamageDealt);
}

int32 ABattlefield::GetLives(){
	return Lives; 
}

float ABattlefield::GetLivesPercentage()
{
	return float(Lives) / float(InitLives);
}

bool ABattlefield::LivesChangedSinceLastCheck()
{
    if (LivesChanged){
        LivesChanged = false;
        return true;
    }
    else return false;
}

void ABattlefield::KillTeam()
{
    TeamActive = false;
    for (auto Creep : Creeps){
        Creep->DestroyCreep();
    }
    MusicStop();
}

bool ABattlefield::IsActive()
{
    return TeamActive;
}

void ABattlefield::TeamDefeated()
{
    KillTeam();
    TeamDefeatedDelegate.Execute(TeamID);
}

//Spawn Creep
//Spawn a new creep of a given type
bool ABattlefield::SpawnCreep(TSubclassOf<ABasicCreep> CreepType, int32 ownerID)
{
	UWorld* const World = GetWorld();
	if (World && CreepType){
		FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = Instigator;
        FRotator SpawnRotation = FRotator::ZeroRotator;
        for (auto SpawnPoint : CreepSpawnPoints){
			FVector SpawnLocation = SpawnPoint->GetActorLocation();
		    ABasicCreep* const NewCreep = World->SpawnActor<ABasicCreep>(CreepType,
                SpawnLocation, SpawnRotation, SpawnParams);
		    if (NewCreep){
                NewCreep->SetMaxHP(NewCreep->DefaultHP * CreepMultiplierHP);
			    Creeps.Add(NewCreep);
				if (TeamID == 1)
					NewCreep->SoundLocation = FVector(23080, -460, 830);
				else if (TeamID == 0)
					NewCreep->SoundLocation = FVector(-7220, -460, 830);
			    NewCreep->OwnerID = ownerID;
			    NewCreep->EnemyTeamID = TeamID;
                NewCreep->CreepKilledDelegate.BindUObject(this, &ABattlefield::OnCreepKilled);
                NewCreep->CreepFinishedDelegate.BindUObject(this, &ABattlefield::OnCreepFinished);
                NewCreep->SpawnDefaultController();
				SpawnPoint->SpawnCreep();

				NewCreep->FinishedInit = true;
		    }
        }
        return true;
	}
    return false;
}

APawn* ABattlefield::SpawnPlaceholder(TSubclassOf<APawn> PlaceholderType, FRect2D Coords)
{
	UWorld* const World = GetWorld();
	if (World && PlaceholderType)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		FRotator SpawnRotation(0.f, Coords.Rotation, 0.f);
		FVector SpawnLocation(Coords.Centre.X, Coords.Centre.Y, 830.f);
		APawn* const PlaceHolder = World->SpawnActor<APawn>(PlaceholderType,
			SpawnLocation, SpawnRotation, SpawnParams);
		return PlaceHolder;
	}
	return nullptr;
}


//Build Tower
//Spawn a new tower of a given type for a given player at given coordinates
//Add the tower to the tower list
//Return false if there's a problem
bool ABattlefield::BuildTower(TSubclassOf<ABasicTower> TowerType, FRect2D TowerCoords, int32 PlayerID, int32 TowerID)
{
	UWorld* const World = GetWorld();
	if (World && TowerType){
		FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = Instigator;
		SpawnParams.bNoCollisionFail = false;
		SpawnParams.bNoFail = false;
		FRotator SpawnRotation(0.f, TowerCoords.Rotation, 0.f);
		FVector SpawnLocation(TowerCoords.Centre.X, TowerCoords.Centre.Y, 830.f);
		ABasicTower* const NewTower = World->SpawnActor<ABasicTower>(TowerType,
            SpawnLocation, SpawnRotation, SpawnParams);
		if (NewTower){
			if (TeamID == 1)
				NewTower->SoundLocation = FVector(23080, -460, 830);
			else if (TeamID == 0)
				NewTower->SoundLocation = FVector(-7220, -460, 830);
			Towers.Add(NewTower);
            NewTower->OwnerID = PlayerID;
            NewTower->TeamID = TeamID;
			NewTower->TowerID = TowerID;
			NewTower->TowerRemovedDelegate.BindUObject(this, &ABattlefield::OnTowerRemoved);
            //NewTower->SpawnDefaultController();
			return true;
		}
	}
    return false;
}

void ABattlefield::StartRemoveTower(int32 TowerID)
{
	for (auto Tower : Towers){
		if (Tower->TowerID == TowerID){
			Tower->StartRemove();
			return;
		}
	}
}

void ABattlefield::CancelRemoveTower(int32 TowerID)
{
	for (auto Tower : Towers){
		if (Tower->TowerID == TowerID){
			Tower->CancelRemove();
			return;
		}
	}
}

void ABattlefield::OnCreepKilled(ABasicCreep* Creep, ABasicTower* Tower)
{
    CreepKilledDelegate.Execute(Creep, Tower);
}

void ABattlefield::OnCreepFinished(ABasicCreep* Creep)
{
    InflictDamage(Creep->TeamDamage);
    CreepFinishedDelegate.Execute(Creep);
}

void ABattlefield::OnTowerRemoved(ABasicTower* Tower)
{
	Towers.RemoveSingle(Tower);
	TowerRemovedDelegate.Execute(Tower);
}

bool ABattlefield::BlocksPath(FRect2D Coords, TSubclassOf<AActor> FakeTowerType, TSubclassOf<APawn> FakeCreepType, int32 PlayerID)
{
    bool ret(false);
    UWorld* const World = GetWorld();
    FVector Dest(CreepDestinationPoints[0]->GetActorLocation() - FVector(0, 0, 2230.f));

    // Create tower
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.Instigator = Instigator;
    FRotator SpawnRotation(0.f, Coords.Rotation, 0.f);
    FVector SpawnLocation(Coords.Centre.X, Coords.Centre.Y, 840.f - 2230.f);
    AActor* const NewFakeTower = World->SpawnActor<AActor>(FakeTowerType, SpawnLocation, SpawnRotation, SpawnParams);

    // Create the creeps if necessary
    if (!FakeCreeps.Num())
        for (auto& SpawnPoint : CreepSpawnPoints)
        {
            FVector SpawnLocation(SpawnPoint->GetActorLocation() - FVector(0, 0, 2230.f));
            ACharacter* const NewFakeCreep = World->SpawnActor<ACharacter>(FakeCreepType, SpawnLocation, SpawnRotation, SpawnParams);
            NewFakeCreep->SpawnDefaultController();
            FakeCreeps.Add(NewFakeCreep);
        }

    for (auto& NewFakeCreep : FakeCreeps)
    {
        AController* const Controller = NewFakeCreep->GetController();
        UNavigationSystem* const NavSys = UNavigationSystem::GetCurrent(Controller->GetWorld());
        NavSys->Build();
        auto Result = NavSys->FindPathSync(FPathFindingQuery(Controller, NavSys->GetNavDataForProps(Controller->GetNavAgentPropertiesRef()), Controller->GetNavAgentLocation(), Dest));
        if (!Result.IsSuccessful() || Result.IsPartial()){
            ret = true;
            break;
        }
    }

    // Destroy tower
    NewFakeTower->Destroy();

    return ret;
}

bool ABattlefield::BlockedLocation(TSubclassOf<ABasicTower> TowerType, FRect2D Coords)
{
	bool Result = false;
    /*
	UBoxComponent* CollisionBox = ConstructObject<UBoxComponent>(UBoxComponent::StaticClass(),
		this, NAME_None, RF_NoFlags, TowerType->GetDefaultObject<ABasicTower>()->CollisionBox);
	CollisionBox->SetAbsolute(true, true, true);
	CollisionBox->SetRelativeLocationAndRotation(FVector(Coords.Centre, 830.f), FRotator(0,0,Rotation));
    */
	TArray<FOverlapResult> Overlaps;
	if (GetWorld()->ComponentOverlapMulti(Overlaps, TowerType->GetDefaultObject<ABasicTower>()->CollisionBox,
		FVector(Coords.Centre, 830.f), FRotator(0, 0, Coords.Rotation), ECC_WorldStatic,
		FComponentQueryParams(TEXT("TestThing"), this),
		FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllDynamicObjects))) Result = true;
	//CollisionBox->DestroyComponent();
	return Result;
}

void ABattlefield::BigRed()
{
	for (auto Creep : Creeps)
	{
		if (Creep->Active)
		{
			Creep->KillCreep();
		}
	}

}

#include <string>
#include <array>
#include <vector>

void ABattlefield::MusicInit(irrklang::ISoundEngine* const Engine)
{
    const bool
        playLooped(true),
        startPaused(true),
        track(true),
        enableSoundEffects(false);
    const irrklang::E_STREAM_MODE streamMode(irrklang::ESM_NO_STREAMING);
    const std::string
        contentDirectory(StringCast<char>(&FPaths::GameContentDir()[0]).Get()),
        directory(contentDirectory + "Audio/"),
        extension(".wav");

    this->Engine = Engine;
    const float pan(TeamID * 2 - 1);

    for (unsigned i(0); i < Layers.size(); ++i)
    {
        Layers[i] = Engine->play2D(&(directory + std::to_string(i + 1) + extension)[0], playLooped, startPaused, track, streamMode, enableSoundEffects);
        if (!Layers[i])
            exit(0);
        Layers[i]->setPan(pan);
        Layers[i]->setVolume(0);
        TargetVolume[i] = 0;
    }

    Configuration = 0;
    
    TargetVolume[0] = TargetVolume[1] = TargetVolume[2] = 1;
}

void ABattlefield::MusicStop()
{
    for (auto& layer : Layers)
        layer->drop();
}

void ABattlefield::MusicTick(float DeltaSeconds)
{
	if (!TeamActive)
		return;
    const float VolumeDelta(.1);

    for (unsigned i(0); i < n_layers; ++i)
    {
        auto volume(Layers[i]->getVolume());
        if (volume < TargetVolume[i])
            Layers[i]->setVolume(volume + VolumeDelta);
        else if (volume > TargetVolume[i])
            Layers[i]->setVolume(volume - VolumeDelta);
    }
}

void ABattlefield::SetMusicTrack(int32 Track)
{
    if (Track == Configuration || Track >= 9)
        return;

    const std::vector<const std::vector<const unsigned>> configurations
    {
        {1, 2, 3},
        {1, 2, 4, 5, 6},
        {1, 2, 3, 4, 5, 6},
        {1, 2, 3, 4, 5, 6, 8, 9},
        {2, 3, 4, 7, 8, 9, 10, 11, 12},
        {1, 2, 3, 4, 7, 9, 10, 11, 12, 13, 15},
        {1, 2, 3, 7, 10, 11, 12, 13, 14, 15, 16},
        {1, 2, 4, 7, 8, 9, 10, 11, 13, 15, 16},
        {1, 2, 3, 7, 8, 10, 11, 13, 15, 16, 17}
    };

    if (Configuration >= 0 && Configuration < 9){
        for (auto layer : configurations[Configuration])
            if (layer > 0 && layer <= TargetVolume.size()) TargetVolume[layer - 1] = 0;
        Configuration = Track;
    }
    if (Configuration >= 0 && Configuration < 9){
        for (auto layer : configurations[Configuration])
            if (layer > 0 && layer <= TargetVolume.size()) TargetVolume[layer - 1] = 1;
    }
}

int32 ABattlefield::GetNTracks()
{
    return 9;
}
