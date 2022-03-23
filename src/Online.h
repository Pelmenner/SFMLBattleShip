#pragma once

#include "Board.h"
#include "EnemyBoard.h"
#include "Connection.h"
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>

class Online
{
private:
	Board *local;
	EnemyBoard *opponent;
	Connection *mult;

	sf::RenderWindow &wnd;
	sf::Font &titleFnt;
	sf::Font &fnt;
	sf::RectangleShape bg;

public:
	Online(sf::RenderWindow &wnd, sf::Font &titleFnt, sf::Font &fnt, Connection *mult);

	void SetShipNum(std::vector <sf::Text> &shipNum, sf::Font &fnt);
	void SetCells(std::vector <sf::RectangleShape> &cells);
	void ResetShipNum(std::vector <sf::Text> &shipNum, std::vector <int> &countShips);

	void InitLocal(const std::string &name);
	void SetOpponentName(const std::string &name);

	bool Play(int localMove);
};

