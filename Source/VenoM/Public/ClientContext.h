#ifndef _SERVER_SELECT_H_
#define _SERVER_SELECT_H_

#include "AllowWindowsPlatformTypes.h"
#include <winsock2.h>
#include "HideWindowsPlatformTypes.h"

class ClientContext  //To store and manage client related information
{
private:
	SOCKET m_Socket;  //accepted socket
	int	playerID;

public:
	int getPlayerID();
	void setPlayerID(int id);
	void SetSocket(SOCKET s);
	SOCKET GetSocket();
	//Constructor
	ClientContext(int id, SOCKET paramSocket);
	//destructor
	~ClientContext();
};
#endif