#include "VenoM.h"
#include "NetworkHandler.h"
#include "PlayerConnectionHandler.h"
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <ctime>

bool NetworkHandler::serverRunning = false;
bool NetworkHandler::clientsConnected = false;
bool NetworkHandler::visionConnected = false;
bool NetworkHandler::gameRunning = false;
int maxClients;

const int DEFAULT_PORT = 19834;
const int DEFAULT_BUFLEN = 512;

SOCKET ListenSocket;
SOCKET VisionSocket;
//std::vector<SOCKET> clients;
SOCKET* clients;
char NetworkHandler::names[6][3]; //TEMPORARY!! or not.
char NetworkHandler::tokens[6][3]; //to identify a player.
int recvBuffLen = DEFAULT_BUFLEN;
bool reconnecting[6] = { false, false, false, false, false, false };
bool bigRedButton[6] = { false, false, false, false, false, false };
bool visionCalibrated = false;

struct addrinfo *result = NULL,
	*ptr = NULL,
	hints;
struct sockaddr *clientInfo = NULL;

NetworkHandler::NetworkHandler()
{	
}


NetworkHandler::~NetworkHandler()
{
}

//Connect
//Connects using sockets and stuff
//It's Milan's code
bool NetworkHandler::setUpServer(int numberOfPlayers)
{
	WSADATA wsaData;
	int iResult;
	maxClients = numberOfPlayers;
	clients = new SOCKET[maxClients];
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("SETTING TO NULL"));
	for (int i = 0; i < maxClients; i++) 
		clients[i] = NULL;

	//Initialize Winsock
	std::cout << "Calling socket dll.\n";
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0){
		std::cout << "Error on starting up Socket.\nError code: " << iResult << std::endl;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error on starting up Socket.\nError code:"));
		return false;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("\nWSAStartup() successful."));
	}

	struct sockaddr_in ServerAddress;
	//Create a socket
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == ListenSocket)
	{
		printf("\nError occurred while opening socket: %ld.", WSAGetLastError());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("fail1"));
	}
	else
	{
		printf("\nsocket() successful.");
	}

	//Cleanup and Init with 0 the ServerAddress
	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));

	//Fill up the address structure
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = INADDR_ANY; //WinSock will supply address
	ServerAddress.sin_port = htons(DEFAULT_PORT);    //comes from commandline

	//Assign local address and port number
	if (SOCKET_ERROR == bind(ListenSocket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)))
	{
		closesocket(ListenSocket);

		printf("\nError occurred while binding.");
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("fail2"));
	}
	else
	{
		printf("\nbind() successful.");
	}

	//Make the socket a listening socket
	if (SOCKET_ERROR == listen(ListenSocket, SOMAXCONN))
	{
		closesocket(ListenSocket);

		printf("\nError occurred while listening.");
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("fail3"));
	}
	else
	{
		printf("\nlisten() successful.");
	}

	u_long NonBlock = 1;
	if (ioctlsocket(ListenSocket, FIONBIO, &NonBlock) == SOCKET_ERROR){
		std::cout << "Setting non blocking failed";
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("fail4"));
	}

	NetworkHandler::serverRunning = true;

	std::cout << "Start listening with maximum input queue available.\n";
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Server Running!"));
	
	return true;
}

bool NetworkHandler::acceptVision()
{
	char recvBuff[DEFAULT_BUFLEN];
	int iResult = 0;
	int iSendResult;
	if (visionConnected)
	{
		return true;
	}

	u_long iMode = 1;// asyn mode
	SOCKET ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, clientInfo, NULL);
	if (ClientSocket != SOCKET_ERROR && ClientSocket != INVALID_SOCKET) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Vision!"));
		iSendResult = send(ClientSocket, "Accepted", 9, 0);
		if (iSendResult == SOCKET_ERROR){
			std::cout << "Sending data failed.\nError code: " << WSAGetLastError() << std::endl;
			closesocket(ClientSocket);
		}
		while( iResult<=0 )
		{
			iResult = recv(ClientSocket, recvBuff, recvBuffLen, 0);
		}
		if (recvBuff[0] == '1') //vision handshake
		{
			VisionSocket = ClientSocket;
			ioctlsocket(VisionSocket, FIONBIO, &iMode); //set to async
			visionConnected = true;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Vision Running!"));
			return true;
		}
	}
	closesocket(ClientSocket);
	return false;
}

bool NetworkHandler::acceptClient(int& id)
{
	char recvBuff[DEFAULT_BUFLEN];
	int iResult = 0;
	int iSendResult;
	id = nextFreeID();
	if (id == -1)
		return false;

	u_long iMode = 1;// asyn mode
	SOCKET ClientSocket = INVALID_SOCKET;
	time_t t = time(0);

	ClientSocket = accept(ListenSocket, clientInfo, NULL);
	if (ClientSocket != SOCKET_ERROR && ClientSocket != INVALID_SOCKET) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Client!"));
		iSendResult = send(ClientSocket, "Accepted", 9, 0);
		if (iSendResult == SOCKET_ERROR){
			std::cout << "Sending data failed.\nError code: " << WSAGetLastError() << std::endl;
			closesocket(ClientSocket);
			return false;
		}
		while (iResult <= 0 )
		{
			iResult = recv(ClientSocket, recvBuff, recvBuffLen, 0);
			if (time(0) - t > 1)
				return false;
		}
		if (recvBuff[0] == '0') //client
		{
			int reconnectedID = -1;
			for (int i = 0; i < id; i++)
			{
				if (/*names[i][0] == recvBuff[1] && names[i][1] == recvBuff[2] && names[i][2] == recvBuff[3] && */
					tokens[i][0] == recvBuff[4] && tokens[i][1] == recvBuff[5] && tokens[i][2] == recvBuff[6])
				{
					reconnectedID = i;
					break;
				}
			}
			if (reconnectedID != -1)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PLAYER IDENTIFIED AND SHOULD BE RECONNECTED!!!!!!!\n"));
				names[reconnectedID][0] = recvBuff[1];
				names[reconnectedID][1] = recvBuff[2];
				names[reconnectedID][2] = recvBuff[3];
				clients[reconnectedID] = ClientSocket;
				reconnecting[reconnectedID] = false;
				return false; // return false so that the PCH doesn't think there's a new player.
			}

			ioctlsocket(ClientSocket, FIONBIO, &iMode); // set to async
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Connection accepted from a client\n"));

			names[id][0] = recvBuff[1];
			names[id][1] = recvBuff[2];
			names[id][2] = recvBuff[3];

			tokens[id][0] = recvBuff[4];
			tokens[id][1] = recvBuff[5];
			tokens[id][2] = recvBuff[6];

			clients[id] = ClientSocket;

			if (nextFreeID() == -1)
				clientsConnected = true;

			return true;
		}
	}
		
	closesocket(ClientSocket);
	return false;
}

void NetworkHandler::processClientSocket(int32 ClientID, TArray<InputCommand>& input)
{ 
	if (reconnecting[ClientID])
	{
		reconnectClient();//std::thread t1(reconnectClient);
		return;
	}
	char recvBuff[DEFAULT_BUFLEN];
	int iResult;
	//int iSendResult;
    if (ClientID <= maxClients && ClientID >= 0){
        iResult = recv(clients[ClientID], recvBuff, recvBuffLen, 0);
        int32 nError = WSAGetLastError();
        if (nError != WSAEWOULDBLOCK&&nError != 0)
        {/*
            std::cout << "Winsock error code: " << nError << "\r\n";
            std::cout << "Client disconnected!\r\n";*/
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Client DC!"));
			reconnecting[ClientID] = true;
			reconnectClient();
			
        }
       /* if (iResult > 0)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Received:"));
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, recvBuff);
        }*/
        for (int counter = 0; counter < iResult / 5; counter++)
        {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, recvBuff);
            char command = recvBuff[counter * 5];
			if (command == RC_BigRedButton)//special handler for the BRB
			{
				DEBUG_MESSAGE("got something");
				if (!bigRedButton[ClientID])
				{
					bigRedButton[ClientID] = true;
					if (!(bigRedButton[3 * (ClientID / 3)] && bigRedButton[3 * (ClientID / 3) + 1] && bigRedButton[3 * (ClientID / 3) + 2])) // if everyone in the same team pressed the button
						continue;
				}
				else
					continue;
				
			}
            int32 paramInt = (unsigned char)(recvBuff[counter * 5 + 1]) | (unsigned char)(recvBuff[counter * 5 + 2]) << 8 | (unsigned char)(recvBuff[counter * 5 + 3]) << 16 | (unsigned char)(recvBuff[counter * 5 + 4]) << 24;
            char* param = static_cast<char*>(static_cast<void*>(&paramInt));
            //DEBUG_MESSAGE(FString::Printf(TEXT("Command is %d %d"), (uint8)command, paramInt));
			InputCommand commandStruct = {(receiveCommand)command,paramInt};
			input.Add(commandStruct);
        }
    }
	return;
}

void NetworkHandler::processVisionSocket(TArray<TowerCoords>& input, TArray<TowerCoords>& removeTowers)
{ 
	char recvBuff[DEFAULT_BUFLEN];
	int iResult;
	iResult = recv(VisionSocket, recvBuff, recvBuffLen, 0);//get stuff from vision

	int nError = WSAGetLastError();
	if (nError != WSAEWOULDBLOCK&&nError != 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Vision DC!"));
		//exit(9); //vision DC
	}
    if (iResult > 0){
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, recvBuff);
    }
	for (int counter = 0; counter < iResult / 3; counter++)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, recvBuff);
		unsigned x = (unsigned char)(recvBuff[counter * 3]);
		unsigned y = (unsigned char)(recvBuff[counter * 3 + 1]);
		unsigned angle = (unsigned char)(recvBuff[counter * 3 + 2]);
		char* paramX = static_cast<char*>(static_cast<void*>(&x));
		char* paramY = static_cast<char*>(static_cast<void*>(&y));

		TowerCoords data = { x/255.0, y/255.0, angle };

		if (angle <= 180)
			input.Add(data);
		else
			removeTowers.Add(data);
		/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Vision received: "));
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, paramX);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, paramY);*/
	}
}


bool NetworkHandler::confirmCalibration()
{
	char recvBuff[DEFAULT_BUFLEN];
	int iResult;
	iResult = recv(VisionSocket, recvBuff, recvBuffLen, 0);//get stuff from vision
	int nError = WSAGetLastError();
	if (nError != WSAEWOULDBLOCK&&nError != 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Vision DC!"));
		//exit(9); //vision DC
	}
	if (iResult > 0){
		return true;
	}
	return false;
}


//String From Binary Array
//Converts a string from a binary array
FString NetworkHandler::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array!
	const std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
	return FString(cstr.c_str());
}

int NetworkHandler::nextFreeID()
{
	for (int i = 0; i < maxClients; i++)
		if (clients[i] == NULL)
			return i;
	return -1;
}


void NetworkHandler::sendCommandToClient(int32 ClientID, OutputData input)
{
	int iResult = 0;
	int iSendResult;
	SOCKET Client = clients[ClientID];
	char command = input.Command;
	char* param = static_cast<char*>(static_cast<void*>(&(input.Value)));

	char message[5] = { command, param[0], param[1], param[2], param[3] };
	iSendResult = send(Client, message, 5, 0);
	if (iSendResult == SOCKET_ERROR){
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("CANNOT SEND A MESSAGE FOR SOME REASON!!\n"));
		reconnecting[ClientID] = true;
		//closesocket(client);
		return;
	}
	return;
}

void NetworkHandler::sendStartToVision()
{
	int iResult = 0;
	int iSendResult;
	char command = 1;

	iSendResult = send(VisionSocket, &command, 1, 0);
}

void NetworkHandler::sendResetToVision()
{
	char recvBuff[DEFAULT_BUFLEN];
	int iResult = 0;
	int iSendResult;
	char command = 0;

	iSendResult = send(VisionSocket, &command, 1, 0);
	iResult = recv(VisionSocket, recvBuff, recvBuffLen, 0);//clear the buffer

}

void NetworkHandler::sendCalibrateToVision()
{
	char recvBuff[DEFAULT_BUFLEN];
	int iResult = 0;
	int iSendResult;
	char command = 2;

	iResult = recv(VisionSocket, recvBuff, recvBuffLen, 0);//clear the buffer

	iSendResult = send(VisionSocket, &command, 1, 0);
}

void NetworkHandler::stopServer()
{
	int iResult = 0;
	int iSendResult;
	char command = 0;
	iSendResult = send(VisionSocket, &command, 1, 0);

	visionCalibrated = false;
	serverRunning = false;
	clientsConnected = false;
	visionConnected = false;
	for (int i = 0; i < 6; i++){
		closesocket(clients[i]);
		reconnecting[i] = false;
		bigRedButton[i] = false;
	}
	closesocket(VisionSocket);
	closesocket(ListenSocket);
	clients = NULL;
	WSACleanup();
	return;
}

void NetworkHandler::restartServer()
{
	int iResult = 0;
	int iSendResult;
	char command = 0;
	iSendResult = send(VisionSocket, &command, 1, 0);
	visionCalibrated = false;

	char recvBuff[DEFAULT_BUFLEN];

	//serverRunning = false;
	clientsConnected = false;
	//visionConnected = false;
	for (int i = 0; i < 6; i++){
		closesocket(clients[i]);
		clients[i] = NULL;
		reconnecting[i] = false;
		bigRedButton[i] = false;
	}

	iResult = recv(VisionSocket, recvBuff, recvBuffLen, 0);//clear the buffer

	//closesocket(VisionSocket);
	//closesocket(ListenSocket);
	//clients = NULL;
	//WSACleanup();
	return;
}

void NetworkHandler::notifyPlayerSlots()
{
	int playersJoined = nextFreeID();
	if (playersJoined == -1)
		playersJoined = 6;
	for (int i = 0; i < playersJoined; i++)
	{
		SOCKET Client = clients[i];
		if (reconnecting[i])
		{
			reconnectClient();//try to reconnect this client and continue;
			continue;
		}
		for (int ii = 0; ii < playersJoined; ii++)
		{
			int iResult = 0;
			int iSendResult;
			char command[5];
			command[0] = SC_NotifyPosition;
			command[1] = ii;
			command[2] = names[ii][0];
			command[3] = names[ii][1];
			command[4] = names[ii][2];
			iSendResult = send(Client, command, 5, 0);
			if (iSendResult == SOCKET_ERROR){
				reconnecting[i] = true;
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("CANNOT SEND A MESSAGE FOR SOME REASON IN LOBBY!!\n"));
				break;
				//closesocket(client);
				return;
			}
		}
	}
}

bool NetworkHandler::reconnectClient()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RECONNECTING A PLAYER!!!!!!!\n"));
	char recvBuff[DEFAULT_BUFLEN];
	int iResult = 0;
	int iSendResult;
	int reconnectedID = -1;
	//id = nextFreeID();
	//if (id == -1)
	//	return false;

	u_long iMode = 1;// asyn mode
	SOCKET ClientSocket = INVALID_SOCKET;
	time_t t = time(0);

	ClientSocket = accept(ListenSocket, clientInfo, NULL);
	if (ClientSocket != SOCKET_ERROR && ClientSocket != INVALID_SOCKET) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("CONNECTION FOUND!!!!!!!\n"));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Client!"));
		iSendResult = send(ClientSocket, "Accepted", 9, 0);
		if (iSendResult == SOCKET_ERROR){
			std::cout << "Sending data failed.\nError code: " << WSAGetLastError() << std::endl;
			closesocket(ClientSocket);
			return false;
		}
		while (iResult <= 0)
		{
			iResult = recv(ClientSocket, recvBuff, recvBuffLen, 0);
			if (time(0) - t > 1)
				return false;
		}
		if (recvBuff[0] == '0') //client
		{
			ioctlsocket(ClientSocket, FIONBIO, &iMode); // set to async
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("HANDSHAKE PASSED!!!!!!!\n"));
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Connection accepted from a client\n"));
			for (int id = 0; id < 6; id++)
			{
				if (/*names[id][0] == recvBuff[1] && names[id][1] == recvBuff[2] && names[id][2] == recvBuff[3] &&*/
					tokens[id][0] == recvBuff[4] && tokens[id][1] == recvBuff[5] && tokens[id][2] == recvBuff[6])
				{
					reconnectedID = id;
					break;
				}
			}
			if (reconnectedID != -1)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PLAYER IDENTIFIED AND SHOULD BE RECONNECTED!!!!!!!\n"));
				names[reconnectedID][0] = recvBuff[1];
				names[reconnectedID][1] = recvBuff[2];
				names[reconnectedID][2] = recvBuff[3];
				clients[reconnectedID] = ClientSocket;
				reconnecting[reconnectedID] = false;
				if (gameRunning)
					sendCommandToClient(reconnectedID, OutputData(SC_GameStarted, 1)); // send the client a message that the game is already running lel.
				return true;
			}
		}
	}

	closesocket(ClientSocket);
	return false;
}