#pragma once
#include "ClientContext.h"

enum sendCommand
{
	SC_AmountOfMoney,
	SC_Lives,
	SC_Quit,
    SC_SwapRequested,
    SC_SwapSuccessful,
    SC_SwapFailed,
    SC_GameStarting,
    SC_GameStarted,
	SC_NotifyPosition,
	SC_WON,
	SC_LOST,
	SC_Income
};

enum receiveCommand
{
    RC_SpawnCreep,
    RC_SelectTower,
    RC_MoveSlot,
    RC_SetReady,
    RC_AcceptSwap,
    RC_DeclineSwap,
	RC_BigRedButton
};

struct TowerCoords {
	float x;
	float y;
	int32 angle;
};

struct InputCommand {
    InputCommand(receiveCommand NCommand, int32 NOperand)
        : Command(NCommand), Operand(NOperand) {}
	receiveCommand Command;
	int32 Operand;
};

struct OutputData {
    OutputData(sendCommand NCommand, int32 NValue)
        : Command(NCommand), Value(NValue) {}
	sendCommand Command;
	int32 Value;
};

class NetworkHandler
{
public:
	static bool serverRunning;
	static bool clientsConnected;
	static bool visionConnected;
	static bool gameRunning;
	static char NetworkHandler::names[6][3];
	static char NetworkHandler::tokens[6][3];
	NetworkHandler();
	~NetworkHandler();
	static void processSockets();
	static void processClientSocket(int32 ClientID, TArray<InputCommand>& input);
	static void processVisionSocket(TArray<TowerCoords>& input, TArray<TowerCoords>& removeTowers);
	static bool confirmCalibration();
	static bool acceptVision();
	static bool acceptClient(int& id);
	static int nextFreeID();
	static FString StringFromBinaryArray(const TArray<uint8>& BinaryArray);
	static bool setUpServer(int numberOfPlayers);
	static void sendCommandToClient(int32 ClientID, OutputData input);
	static void sendStartToVision();
	static void sendResetToVision();
	static void sendCalibrateToVision();
	static void stopServer();
	static void restartServer();
	static void notifyPlayerSlots();
	static bool reconnectClient();
private:
	static int GetSocketSpecificError(SOCKET Socket);
};