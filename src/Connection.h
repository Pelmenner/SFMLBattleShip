#pragma once

#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <string>

#include "RemoteField.h"

class Connection
{
private:
	sf::TcpSocket client;
	sf::TcpListener listener;
	bool connected, listening;

public:
	Connection();

	bool PassiveConnection(int port = 19273);
	bool ActiveConnection(std::string ip, int port = 19273);

	bool SendName(const std::string &name);
	bool ReceiveName(std::string &name);

	bool SendMove(int x, int y);
	bool ReceiveMove(int &x, int &y);
	bool SendResponse(int hit);
	bool ReceiveResponse(int &hit);

	bool SendField(int state[10][10]);
	bool ReceiveField(RemoteField &b);

	bool ReceiveTurn(int &turn);

	void Disconnect();
};