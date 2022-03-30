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

	bool passiveConnection(int port = 19273);
	bool activeConnection(std::string ip, int port = 19273);

	bool sendName(const std::string &name);
	bool receiveName(std::string &name);

	bool sendMove(int x, int y);
	bool receiveMove(int &x, int &y);
	bool sendResponse(int hit);
	bool receiveResponse(int &hit);

	bool sendField(int state[10][10]);
	bool receiveField(RemoteField &b);

	bool receiveTurn(int &turn);

	void disconnect();
};
