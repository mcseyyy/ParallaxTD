// Fill out your copyright notice in the Description page of Project Settings.

#include "VenoM.h"
#include "PlayerStats.h"
#include "Battlefield.h"
#include "PlayerConnectionHandler.h"
#include "VisionDeviceReceiver.h"
#include "CreepMarker.h"
#include "PlayerZone.h"
#include "VenoMGameInstance.h"
#include "PlayGameMode.h"
#include "BasicCreepAIController.h"

#define ADDCREEPBP(x) do{ \
	static ConstructorHelpers::FObjectFinder<UClass> CreepType(L"Class'/Game/Blueprints/Creeps/"L##x##L"."L##x##L"_C'"); \
	if (CreepType.Object != NULL)         \
		CreepTypes.Add(CreepType.Object); \
} while (0)

#define ADDTOWERBP(x) do{ \
	static ConstructorHelpers::FObjectFinder<UClass> TowerType(L"Class'/Game/Blueprints/Towers/"L##x##L"."L##x##L"_C'"); \
	if (TowerType.Object)                 \
		TowerTypes.Add(TowerType.Object); \
} while (0)


void APlayGameMode::AddCreepBP(TCHAR *path)
{
    static ConstructorHelpers::FObjectFinder<UBlueprint> CreepType(path);
    if (CreepType.Object != NULL){
        CreepTypes.Add((UClass*)CreepType.Object->GeneratedClass);
    }
    else{
        DEBUG_MESSAGE("Could not find creep blueprint.");
    }
}
void APlayGameMode::AddTowerBP(TCHAR *path)
{
	static ConstructorHelpers::FObjectFinder<UBlueprint> TowerType(path);
    if (TowerType.Object != NULL) {
		TowerTypes.Add((UClass*)TowerType.Object->GeneratedClass);
    }
	else {
		DEBUG_MESSAGE("Could not find tower blueprint.");
    }
}

APlayGameMode::APlayGameMode(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	// ADDING CREEP BLUEPRINTS
    ADDCREEPBP("Another_Creep");
    ADDCREEPBP("Ball_Creep");
    ADDCREEPBP("Batmobile_Creep");
    ADDCREEPBP("Bowtie_Creep");
    ADDCREEPBP("Bullet_Creep");
    ADDCREEPBP("Car_Creep");
    ADDCREEPBP("Cock_Creep");
    ADDCREEPBP("Crack_Creep");
    ADDCREEPBP("Egg_Creep");
    ADDCREEPBP("Grenade_Creep");
    ADDCREEPBP("Hands_Creep");
    ADDCREEPBP("Helm_Creep");
    ADDCREEPBP("Jellifish_Creep");
    ADDCREEPBP("Lightbulb_Creep");
    ADDCREEPBP("Mike_Creep");
    ADDCREEPBP("Mushrom_Creep");
    ADDCREEPBP("Orbital_Creep");
    ADDCREEPBP("Paper_Creep");
    ADDCREEPBP("Patrick_Creep");
    ADDCREEPBP("Pencil_Creep");
    ADDCREEPBP("Tank_Creep");
    ADDCREEPBP("Tired_Creep");
    ADDCREEPBP("Train_Creep");
    ADDCREEPBP("Wheel_Creep");
	//ADDING TOWER BPs
    ADDTOWERBP("Beyblade_Tower");
    ADDTOWERBP("Bullet_Tower");
    ADDTOWERBP("Butt_Tower");
    ADDTOWERBP("Chinese_Tower");
    ADDTOWERBP("Cone_Tower");
    ADDTOWERBP("Diamond_Tower");
    ADDTOWERBP("Dough_Tower");
    ADDTOWERBP("Drill_Tower");
    ADDTOWERBP("Electricity_Tower");
    ADDTOWERBP("Eye_Tower");
    ADDTOWERBP("Flower_Tower");
    ADDTOWERBP("Fragmented_Tower");
    ADDTOWERBP("Hole_Tower");
    ADDTOWERBP("Hunt_Tower");
    ADDTOWERBP("Michael_Tower");
    ADDTOWERBP("Modern_Tower");
    ADDTOWERBP("Monolith_Tower");
    ADDTOWERBP("Monument_Tower");
    ADDTOWERBP("Pyramid_Tower");
    ADDTOWERBP("Round_Tower");
    ADDTOWERBP("Satellite_Tower");
    ADDTOWERBP("Snake_Tower");
    ADDTOWERBP("Spikey_Tower");
    ADDTOWERBP("Star_Tower");
    ADDTOWERBP("Tangerine_Tower");
    ADDTOWERBP("Thing_Tower");
	ADDTOWERBP("Twisty_Tower");
	ADDTOWERBP("Whirly_Tower");
	ADDTOWERBP("Windmill_Tower");
	ADDTOWERBP("ZZBob_Tower");

    //Add BPs for fake pawns
    static ConstructorHelpers::FObjectFinder<UClass> FakeCreepBlueprint(L"Class'/Game/Blueprints/FakePawns/Fake_Creep.Fake_Creep_C'");
    if (!FakeCreepBlueprint.Object)
        FakeCreepType = nullptr;
    else
        FakeCreepType = FakeCreepBlueprint.Object;

    static ConstructorHelpers::FObjectFinder<UClass> FakeTowerBlueprint(L"Class'/Game/Blueprints/FakePawns/Fake_Tower.Fake_Tower_C'");
    if (!FakeTowerBlueprint.Object)
        FakeTowerType = nullptr;
    else
        FakeTowerType = FakeTowerBlueprint.Object;


	//Add BPs for placeholders
	//no money
	static ConstructorHelpers::FObjectFinder<UClass> NoMoneyBP(L"Class'/Game/Blueprints/TowerPlaceholder/NoMoney_PlaceHolder.NoMoney_PlaceHolder_C'");
	if (!NoMoneyBP.Object)
		PlaceHolder_NoMoney = nullptr;
	else
		PlaceHolder_NoMoney = NoMoneyBP.Object;
	//no path
	static ConstructorHelpers::FObjectFinder<UClass> NoPathBP(L"Class'/Game/Blueprints/TowerPlaceholder/NoPath_PlaceHolder.NoPath_PlaceHolder_C'");
	if (!NoPathBP.Object)
		PlaceHolder_NoPath = nullptr;
	else
		PlaceHolder_NoPath = NoPathBP.Object;

    GameActive = false;
}

void APlayGameMode::GetSeamlessTravelActorList(bool bToEntry, TArray<AActor*>& ActorList)
{
    Super::GetSeamlessTravelActorList(bToEntry, ActorList);
    if(PCH) ActorList.Add(PCH);
    if(VDR) ActorList.Add(VDR);
}

void APlayGameMode::PostSeamlessTravel()
{
    Super::PostSeamlessTravel();
    for (TActorIterator<APlayerConnectionHandler> PCHItr(GetWorld()); PCHItr; ++PCHItr){
        PCH = *PCHItr;
    }
    for (TActorIterator<AVisionDeviceReceiver> VDRItr(GetWorld()); VDRItr; ++VDRItr){
        VDR = *VDRItr;
    }
}

void APlayGameMode::RemoveSplitscreen()
{
    if (GameActive) return;
    TArray<ULocalPlayer*> PlayersToRemove;
    for (FConstPlayerControllerIterator Itr =
            GEngine->GameViewport->GetWorld()->GetPlayerControllerIterator();
            Itr; Itr++)
    {
        APlayerController* Controller = *Itr;
        if (Controller && Controller->IsLocalController() && !Controller->IsPrimaryPlayer()){
            ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Controller->Player);
            if (LocalPlayer){
                Controller->PawnPendingDestroy(Controller->GetPawn());
                PlayersToRemove.Add(LocalPlayer);
            }
        }
    }
    //for(auto Player : PlayersToRemove) GEngine->GameViewport->RemovePlayer(Player);
    UGameInstance* GameInstance = GetWorld()->GetGameInstance();
    for (auto Player : PlayersToRemove) GameInstance->RemoveLocalPlayer(Player);
}

void APlayGameMode::ReturnToMenu()
{
    if(GameActive) return;
    Engine->drop();
    VDR->ShutDownVisionServer();
    PCH->ShutDownPlayerServer();
	NetworkHandler::stopServer();
    RemoveSplitscreen();
	GetWorld()->SeamlessTravel(MAP_MENU);
}

void APlayGameMode::ReturnToLobby()
{
    if(GameActive) return;
    Engine->drop();
    //VDR->ShutDownVisionServer(); //not shutting down anything
    //PCH->ShutDownPlayerServer(); //same
	NetworkHandler::restartServer();
	PCH->SetupPlayerServer(MaxPlayers); //This is necessary to wipe all the old player data. Without this, bad things happen.
    RemoveSplitscreen();
	GetWorld()->SeamlessTravel(MAP_PRELOBBY);
}

void APlayGameMode::InitPlayerZones()
{
	NetworkHandler::gameRunning = true;
    TArray<FCoordMap> ZoneCoords;
    TArray<FRect2D> TeamBounds;
    ZoneCoords.AddUninitialized(MaxPlayers);
    TeamBounds.AddUninitialized(MaxTeams);
    int32 TeamSetCount = 0,
          ZoneSetCount = 0;
    for (TActorIterator<APlayerZone> ZoneItr(GetWorld()); ZoneItr; ++ZoneItr){
        if (ZoneItr->SlotID == -1 && ZoneItr->TeamID < MaxTeams && ZoneItr->TeamID >= 0){
            TeamBounds[ZoneItr->TeamID] = ZoneItr->Bounds;
            TeamSetCount++;
        }
        else if (ZoneItr->TeamID < MaxTeams && ZoneItr->SlotID < MaxSlots &&
                 ZoneItr->TeamID >= 0 && ZoneItr->SlotID >= 0)
        {
            int32 PlayerID = ZoneItr->TeamID*MaxSlots + ZoneItr->SlotID;
            ZoneCoords[PlayerID].Game = ZoneItr->Bounds;
            ZoneSetCount++;
        }
        else DEBUG_MESSAGE(TEXT("Zone has invalid IDs."));
    }
    if (TeamSetCount != MaxTeams) DEBUG_MESSAGE(TEXT("Not enough Team Bounds set."));
    if (ZoneSetCount != MaxPlayers) DEBUG_MESSAGE(TEXT("Not enough Player Zones set."));
    for (int32 TeamID = 0; TeamID < MaxTeams; TeamID++){
        for (int32 SlotID = 0; SlotID < MaxSlots; SlotID++){
            int32 PlayerID = (TeamID * MaxSlots) + SlotID;
            ZoneCoords[PlayerID].Real = RescaleRect(
                TeamBounds[TeamID], TEAM_BOUNDS_2[TeamID], ZoneCoords[PlayerID].Game);
        }
    }
    VDR->SetPlayerZones(ZoneCoords);
}

void APlayGameMode::InitTeams()
{
	NetworkHandler::gameRunning = true;
	UWorld *const World = GetWorld();
    auto GameInstance = Cast<UVenoMGameInstance>(World->GetGameInstance());
    FGameLevelData LevelData = GameInstance->LevelData;
    // Load game elements from map
    TArray<TArray<ACreepMarker*>> CreepMarkers;
    for(int32 C = 0; C < MaxTeams; C++)
		CreepMarkers.Add(TArray<ACreepMarker*>());
    for (TActorIterator<ACreepMarker> MarkItr(World); MarkItr; ++MarkItr){
        CreepMarkers[MarkItr->TeamID].Add(*MarkItr);
    }

    Engine = irrklang::createIrrKlangDevice();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = Instigator;
    // Create team battlefields
    for (int32 TeamID = 0; TeamID < MaxTeams; TeamID++){
	    ABattlefield* NewBattlefield = World->SpawnActor<ABattlefield>(
            ABattlefield::StaticClass(), SpawnParams);
	    if (NewBattlefield){
            Teams.Add(NewBattlefield);
            NewBattlefield->InitBattlefield(TeamID, LevelData.StartingLives, CreepMarkers[TeamID], FakeCreepType, Engine);
            NewBattlefield->CreepKilledDelegate.BindUObject(this, &APlayGameMode::CreepKilledEvent);
            NewBattlefield->CreepFinishedDelegate.BindUObject(this, &APlayGameMode::CreepFinishedEvent);
			NewBattlefield->TeamDefeatedDelegate.BindUObject(this, &APlayGameMode::TeamDefeatedEvent);
			NewBattlefield->TowerRemovedDelegate.BindUObject(this, &APlayGameMode::TowerRemovedEvent);
			NewBattlefield->TargetTeamID = (TeamID + 1 == MaxTeams) ? 0 : TeamID + 1;
	    }
        else DEBUG_MESSAGE(FString::Printf(TEXT("Error creating team %d."), TeamID));
    }
    // Create players
    for (int32 PlayerID = 0; PlayerID < MaxPlayers; PlayerID++){
	    APlayerStats* NewPlayer = World->SpawnActor<APlayerStats>(
            APlayerStats::StaticClass(), SpawnParams);
	    if (NewPlayer){
            Players.Add(NewPlayer);
            NewPlayer->InitPlayer(LevelData.StartingMoney, LevelData.StartingIncome);
	    }
        else DEBUG_MESSAGE(FString::Printf(TEXT("Error creating player %d."), PlayerID));
    }

    Engine->setAllSoundsPaused(false);
}



void APlayGameMode::PlayerGameAction(int32 PlayerID, receiveCommand Action, int32 Value)
{
    APlayerStats* Player = Players[PlayerID];
    switch (Action){
        case RC_SpawnCreep: {
            if (CreepTypes.IsValidIndex(Value)){
                TSubclassOf<ABasicCreep> CreepToSpawn = CreepTypes[Value];
                if (CreepToSpawn) {
                    if (Player->CanPurchaseCreep(CreepToSpawn)){
                        int32 TargetTeamID = Teams[GET_TEAM_ID(PlayerID)]->TargetTeamID;
                        if (Teams[TargetTeamID]->SpawnCreep(CreepToSpawn, PlayerID)){
                            Player->FinalizePurchaseCreep(CreepToSpawn);
                        }
                    }
                }
            }
            break;
        }
        case RC_SelectTower: {
            TSubclassOf<ABasicTower> SelectedTower = TowerTypes[Value];
            if (SelectedTower) Player->SelectTower(SelectedTower);
            break;
        }
		case RC_BigRedButton:
		{	
			Teams[GET_TEAM_ID(PlayerID)]->BigRed();
			break;
		}
        default:
            break;
    }
}

void APlayGameMode::PlayerPostGameAction(int32 PlayerID, receiveCommand Action, int32 Value)
{
}

void APlayGameMode::StartPlay()
{
    Super::StartPlay();
    GameTime = 0;
	if (!NetworkHandler::serverRunning) DebugStart();
    auto GameInstance = Cast<UVenoMGameInstance>(GetWorld()->GetGameInstance());
    MaxTeams = GameInstance->LevelData.NumberOfTeams;
    MaxSlots = GameInstance->LevelData.PlayersPerTeam;
    MaxPlayers = MaxTeams * MaxSlots;
    WaveTime = GameInstance->LevelData.WaveTime;
    NextWave = WaveTime;
    InitPlayerZones();
    InitTeams();
    for (int32 c = 0; c < MaxPlayers; c++) PCH->SendData(c, OutputData(SC_GameStarted, 0));
    GameActive = true;
	NetworkHandler::gameRunning = true;
}

void APlayGameMode::DebugStart()
{
	DEBUG_MESSAGE("Server offline, starting in debug mode.");
	UWorld* const World = GetWorld();
	if (World){
        auto GameInstance = Cast<UVenoMGameInstance>(GetWorld()->GetGameInstance());
        MaxTeams = GameInstance->LevelData.NumberOfTeams;
        MaxSlots = GameInstance->LevelData.PlayersPerTeam;
        MaxPlayers = MaxTeams * MaxSlots;
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
}

void APlayGameMode::HandleClientInput()
{
    if (PCH && PCH->Update() == PCH_Full){
        for (int PlayerID = 0; PlayerID < MaxPlayers; PlayerID++){
            TArray<InputCommand> PlayerCommands = PCH->GetInput(PlayerID);
            for (auto Command : PlayerCommands){
                if (Teams[GET_TEAM_ID(PlayerID)]->IsActive()){
                    PlayerGameAction(PlayerID, Command.Command, Command.Operand);
                }
                else{
                    PlayerPostGameAction(PlayerID, Command.Command, Command.Operand);
                }
            }
        }
	}
}

void APlayGameMode::HandleVisionInput()
{
	if (VDR && VDR->Update() == VDR_Connected)
	{
		for (int PlayerID = 0; PlayerID < MaxPlayers; PlayerID++)
		{
            if (!Teams[GET_TEAM_ID(PlayerID)]->IsActive()) 
				continue;
            APlayerStats* Player = Players[PlayerID];
			//TArray<TowerData> NewTowers = VDR->GetTowers(PlayerID, TS_New,false);
			TArray<TowerData>& PlayerTowers = VDR->TowerList[PlayerID];
            TSubclassOf<ABasicTower> TowerToBuild = Player->SelectedTower;
			///////
			//for (auto &Tower : PlayerTowers)
			//{
			//	if (Tower.Status != TS_Queued)
			//		continue;
			//	if (Player->CanPurchaseSelectedTower())
			//	{
			//		Tower.Placeholder->Destroy();
			//		Tower.Placeholder = nullptr;
			//		if (Teams[GET_TEAM_ID(PlayerID)]->BuildTower(
			//			TowerToBuild, Tower.GameCoords, PlayerID, Tower.ID))
			//		{
			//			Player->FinalizePurchaseSelectedTower();
			//			Tower.Status = TS_Normal;
			//		
			//		}
			//	}
			//}
			///////
			for (auto& Tower : PlayerTowers)
			{
				if (Tower.Status == TS_New || Tower.Status == TS_Queued){
					if (Player->CanPurchaseSelectedTower()){
						if (Tower.Status == TS_Queued){
                            Tower.Placeholder->Destroy();
                            Tower.Placeholder = NULL;
						}
						if (!Teams[GET_TEAM_ID(PlayerID)]->BlocksPath(Tower.GameCoords, FakeTowerType, FakeCreepType, PlayerID)){
                            if (Teams[GET_TEAM_ID(PlayerID)]->BlockedLocation(TowerToBuild, Tower.GameCoords)) DEBUG_MESSAGE("Tower blocked!");
							if (Teams[GET_TEAM_ID(PlayerID)]->BuildTower(TowerToBuild, Tower.GameCoords, PlayerID, Tower.ID)){
								Player->FinalizePurchaseSelectedTower();
								Tower.Status = TS_Normal;
							}
							else{
								Tower.Placeholder = Teams[GET_TEAM_ID(PlayerID)]->SpawnPlaceholder(PlaceHolder_NoPath, Tower.GameCoords);
								Tower.Status = TS_Blocked;
							}
						}
						else{
							Tower.Placeholder = Teams[GET_TEAM_ID(PlayerID)]->SpawnPlaceholder(PlaceHolder_NoPath, Tower.GameCoords);
							Tower.Status = TS_Blocked;
						}
					}
					else if (Tower.Status == TS_New)
					{
						//spawn a placeholder only if there is none
						Tower.Placeholder = Teams[GET_TEAM_ID(PlayerID)]->SpawnPlaceholder(PlaceHolder_NoMoney, Tower.GameCoords);
						Tower.Status = TS_Queued;
					}
				}
			}
			//TArray<TowerData> RemovedTowers = VDR->GetTowers(PlayerID, TS_Removed);
			//for (auto Tower : RemovedTowers){
			//	Teams[GET_TEAM_ID(PlayerID)]->StartRemoveTower(Tower.ID);
			//	VDR->ResetTowerStatus(PlayerID, Tower.ID);
			//}
			//TArray<TowerData> RestoredTowers = VDR->GetTowers(PlayerID, TS_Restored);
			//for (auto Tower : RestoredTowers){
			//	Teams[GET_TEAM_ID(PlayerID)]->CancelRemoveTower(Tower.ID);
			//	VDR->ResetTowerStatus(PlayerID, Tower.ID);
			//}
		}
	}

	//if (VDR && VDR->Update() == VDR_Connected){
	//	for (int PlayerID = 0; PlayerID < MaxPlayers; PlayerID++){
	//		if (!Teams[GET_TEAM_ID(PlayerID)]->IsActive()) continue;
	//		APlayerStats* Player = Players[PlayerID];
	//		TArray<TowerData> NewTowers = VDR->GetTowers(PlayerID, TS_New, false);
	//		TSubclassOf<ABasicTower> TowerToBuild = Player->SelectedTower;
	//		for (auto Tower : NewTowers){
	//			if (Player->CanPurchaseSelectedTower()){
	//				if (TowerToBuild){
	//					if (Teams[GET_TEAM_ID(PlayerID)]->BuildTower(
	//						TowerToBuild, Tower.GameCoords, PlayerID, Tower.ID))
	//					{
	//						Player->FinalizePurchaseSelectedTower();
	//						VDR->ResetTowerStatus(PlayerID, Tower.ID);
	//					}
	//				}
	//			}
	//			else break;
	//		}
	//		TArray<TowerData> RemovedTowers = VDR->GetTowers(PlayerID, TS_Removed);
	//		for (auto Tower : RemovedTowers){
	//			Teams[GET_TEAM_ID(PlayerID)]->StartRemoveTower(Tower.ID);
	//			VDR->ResetTowerStatus(PlayerID, Tower.ID);
	//		}
	//		TArray<TowerData> RestoredTowers = VDR->GetTowers(PlayerID, TS_Restored);
	//		for (auto Tower : RestoredTowers){
	//			Teams[GET_TEAM_ID(PlayerID)]->CancelRemoveTower(Tower.ID);
	//			VDR->ResetTowerStatus(PlayerID, Tower.ID);
	//		}
	//	}
	//}
}

void APlayGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
	if (GameActive){
		HandleClientInput();
		HandleVisionInput();
        GameTime += DeltaSeconds;
        if (GameTime >= NextWave){
            for (auto Player : Players) Player->WaveTick();
            NextWave += WaveTime;
        }
        int32 TeamID;
        for (int32 PlayerID = 0; PlayerID < MaxPlayers; PlayerID++){
            TeamID = PlayerID / MaxSlots;
            if (Teams[TeamID]->IsActive()){
                if (Players[PlayerID]->MoneyChangedSinceLastCheck()){
                    PCH->UpdatePlayerMoney(PlayerID, Players[PlayerID]->GetMoney());
                }
                if (Players[PlayerID]->IncomeChangedSinceLastCheck()){
                    PCH->UpdatePlayerIncome(PlayerID, Players[PlayerID]->GetIncome());
                }
                if (Teams[TeamID]->LivesChangedSinceLastCheck()){
                    for (int32 Slot = 0; Slot < MaxSlots; Slot++){
                        PCH->UpdatePlayerLives((MaxSlots*TeamID)+Slot, Teams[TeamID]->GetLives());
                    }
                } 
            }
        }
    }
}

ABattlefield* APlayGameMode::GetTeam(int32 TeamID)
{
    return Teams[TeamID];
}
APlayerStats* APlayGameMode::GetPlayer(int32 PlayerID)
{
    return Players[PlayerID];
}

void APlayGameMode::CreepKilledEvent(ABasicCreep* Creep, ABasicTower* Tower)
{
    OnCreepKilled(Creep, Tower);
    if (Tower){
        Players[Tower->OwnerID]->AddMoney(Creep->Reward);
        Players[Tower->OwnerID]->Kills += Creep->Reward;
    }
}

void APlayGameMode::CreepFinishedEvent(ABasicCreep* Creep)
{
    OnCreepFinished(Creep);
}
void APlayGameMode::TeamDefeatedEvent(int32 TeamID)
{
    OnTeamDefeated(TeamID);
    for (int32 PlayerID = TeamID*MaxSlots; PlayerID < (TeamID + 1)*MaxSlots; PlayerID++){
        PCH->SendData(PlayerID, OutputData(SC_LOST, 0));
    }
    int32 FirstTeamID = -1;
    int32 LastTeamID = -1;
    for (auto Team : Teams){
        if (Team->IsActive()){
            if (LastTeamID == -1) FirstTeamID = Team->TeamID;
            else Teams[LastTeamID]->TargetTeamID = Team->TeamID;
            LastTeamID = Team->TeamID;
        }
    }
    Teams[LastTeamID]->TargetTeamID = FirstTeamID;
    if (FirstTeamID == LastTeamID && FirstTeamID >= 0) TeamVictoryEvent(LastTeamID);
}
void APlayGameMode::TeamVictoryEvent(int32 TeamID)
{
	VDR->SendResetToVision();
	GameActive = false;
	auto GameInstance = Cast<UVenoMGameInstance>(GetWorld()->GetGameInstance());
	GameInstance->WinningTeam = TeamID;
    OnTeamVictory(TeamID);
    Teams[TeamID]->KillTeam();
    for (int32 PlayerID = TeamID*MaxSlots; PlayerID < (TeamID + 1)*MaxSlots; PlayerID++){
        PCH->SendData(PlayerID, OutputData(SC_WON, 0));
    }
}

void APlayGameMode::TowerRemovedEvent(ABasicTower* Tower)
{
	if (VDR){
		if (Tower->FinishedSpawning){
			Players[Tower->OwnerID]->AddMoney((Tower->Cost)/2);
		}
		else{
			Players[Tower->OwnerID]->AddMoney(Tower->Cost);
		}
		VDR->PurgeTower(Tower->TowerID);
	}
}
