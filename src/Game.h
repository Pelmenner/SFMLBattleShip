#pragma once

#include"SFML/Graphics.hpp"
#include "Board.h"
#include "Connection.h"

class Game
{
private:
	int gameMode = 0;
	Board *board1, *board2;
	sf::RenderWindow &window;
	sf::Font &titleFont;
	sf::Font &fnt;
	sf::RectangleShape hider;
	sf::RectangleShape bg;

	sf::Texture *restartTexture;
	sf::Texture *exitTexture;
	sf::Texture *randomTexture;
	sf::Texture *acceptTexture;
	sf::Texture *clearTexture;

public:
	Game(sf::RenderWindow &wnd, sf::Font &titleFont, sf::Font &fnt);

	void resetShipNum(std::vector<sf::Text> &shipNum, std::vector<int> &countShips);
	void setShipNum(std::vector<sf::Text> &shipNum, sf::Font &fnt);
	void setBlocks(std::vector<sf::RectangleShape> &blocks);
	void initPlayer(Board *board);
	void setNames(const std::string &name1, const std::string &name2);
	void initPlayers();

	bool playLocal();

	std::pair <Board *, Board *> GetBoards();
};