#include "VenoM.h"
#include "ClientContext.h"


//Get/Set calls
int ClientContext::getPlayerID()
{
	return playerID;
}

void ClientContext::setPlayerID(int id)
{
	playerID = id;
}

void ClientContext::SetSocket(SOCKET s)
{
	m_Socket = s;
}

SOCKET ClientContext::GetSocket()
{
	return m_Socket;
}

//Constructor
ClientContext::ClientContext(int id, SOCKET paramSocket)
{
	m_Socket = paramSocket;
	playerID = id;
}

//destructor
ClientContext::~ClientContext()
{
	closesocket(m_Socket);
}