#include "Connection.h"

Connection::Connection() : connected(false), listening(false)
{
	client.setBlocking(false);
	listener.setBlocking(false);
}

bool Connection::PassiveConnection(int port)
{
	if (connected)
		return true;

	if ((listening && listener.getLocalPort() != port) || !listening)
	{
		if (listener.listen(port) != sf::Socket::Done)
		{
			listening = false;
			return false;
		}
		else
			listening = true;
	}

	if (listener.accept(client) != sf::Socket::Done)
	{
		connected = false;
		return false;
	}
	else
	{
		connected = true;
		return true;
	}
}

bool Connection::ActiveConnection(std::string ip, int port)
{
	if (connected)
		return true;

	if (listening)
	{
		listener.close();
		listening = false;
	}

	client.setBlocking(true);

	if (client.connect(ip, port) != sf::Socket::Done)
		connected = false;
	else
		connected = true;
	
	client.setBlocking(false);

	return connected;
}

bool Connection::SendName(const std::string &name)
{
	sf::Packet pkt;
	pkt << sf::String(name);

	return client.send(pkt) == sf::Socket::Done;
}

bool Connection::ReceiveName(std::string &name)
{
	if (!connected)
		return false;

	sf::Packet pkt;
	sf::Socket::Status status = client.receive(pkt);
	sf::String str;

	switch (status)
	{
	case sf::Socket::Done:
		pkt >> str;
		name = str.toAnsiString();

		return true;
	case sf::Socket::Partial:
		while (status == sf::Socket::Partial)
			status = client.receive(pkt);

		pkt >> str;
		name = str.toAnsiString();

		return true;
	default:
		return false;
	}
}

bool Connection::SendMove(int x, int y)
{
	if (!connected)
		return false;

	sf::Packet pkt;
	pkt << static_cast <sf::Int8>(x) << static_cast <sf::Int8>(y);

	return client.send(pkt) == sf::Socket::Done;
}

bool Connection::ReceiveMove(int &x, int &y)
{
	if (!connected)
		return false;

	sf::Packet pkt;
	sf::Socket::Status status = client.receive(pkt);
	sf::Int8 buf_x = -1, buf_y = -1;

	switch (status)
	{
	case sf::Socket::Done:
		pkt >> buf_x >> buf_y;
		x = static_cast <int>(buf_x);
		y = static_cast <int>(buf_y);

		return true;
	case sf::Socket::Partial:
		while (status == sf::Socket::Partial)
			status = client.receive(pkt);

		pkt >> buf_x >> buf_y;
		x = static_cast <int>(buf_x);
		y = static_cast <int>(buf_y);

		return true;
	default:
		return false;
	}
}

bool Connection::SendResponse(int hit)
{
	if (!connected)
		return false;

	sf::Packet pkt;
	pkt << static_cast <sf::Int16>(hit);

	return client.send(pkt) == sf::Socket::Done;
}

bool Connection::ReceiveResponse(int &hit)
{
	if (!connected)
		return false;

	sf::Packet pkt;
	sf::Socket::Status status = client.receive(pkt);
	sf::Int16 buf_hit = -1;

	switch (status)
	{
	case sf::Socket::Done:
		pkt >> buf_hit;
		hit = static_cast <int>(buf_hit);
		return true;
	case sf::Socket::Partial:
		while (status == sf::Socket::Partial)
			status = client.receive(pkt);

		pkt >> buf_hit;
		hit = static_cast <int>(buf_hit);
		return true;
	default:
		return false;
	}
}

bool Connection::SendField(int state[10][10])
{
	if (!connected)
		return false;

	sf::Packet pkt;
	for (int i = 0; i < 10; ++i)
		for (int j = 0; j < 10; ++j)
			pkt << sf::Int16(state[i][j]);

	return client.send(pkt) == sf::Socket::Done;
}

bool Connection::ReceiveField(RemoteField &b)
{
	if (!connected)
		return false;

	sf::Packet pkt;
	sf::Socket::Status status = client.receive(pkt);

	switch (status)
	{
	case sf::Socket::Done:
		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				sf::Int16 buf = 0;
				pkt >> buf;
				if (buf == 1 && b[i][j].getFillColor() == blackColor)
					b[i][j].setFillColor(greenColor);
			}
		}
		return true;
	case sf::Socket::Partial:
		while (status == sf::Socket::Partial)
			status = client.receive(pkt);

		for (int i = 0; i < 10; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				sf::Int16 buf = 0;
				pkt >> buf;
				if (buf == 1 && b[i][j].getFillColor() == blackColor)
					b[i][j].setFillColor(greenColor);
			}
		}
		return true;
	default:
		return false;
	}
}

bool Connection::ReceiveTurn(int &turn)
{
	if (!connected)
		return false;

	sf::Clock clock;
	sf::Packet pkt = {};
	sf::Socket::Status status = client.receive(pkt);
	while (status != sf::Socket::Done)
	{
		status = client.receive(pkt);
		if (status != sf::Socket::Done && status != sf::Socket::Partial && clock.getElapsedTime().asMilliseconds() > 5000)
			return false;
	}

	sf::Int16 buf;
	pkt >> buf;
	turn = buf;

	return true;
}

void Connection::Disconnect()
{
	if (listening)
	{
		listener.close();
		listening = false;
	}

	if (connected)
	{
		client.disconnect();
		connected = false;
	}
}
