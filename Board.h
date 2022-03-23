#pragma once

#include "Colors.h"

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <random>
#include <ctime>

class Board
{
private:
	struct ship
	{
		int length;
		int direction; // horizontal - 1, vertical - 0
		sf::Vector2i pos;
		int health;

		ship(sf::Vector2i start, sf::Vector2i end);
		ship() : length(0), direction(0), health(0) {};
	};

	std::vector <ship> ships;
	std::vector <int> count_ships;
	std::string name;
	std::vector <std::vector <sf::RectangleShape>> cells;
	sf::Text letters[10] = {};
	sf::Text numbers[10] = {};
	sf::Font font;

	int checkHit(sf::Vector2i position);
	void surroundKilled(int index);
	void getNeighbours(sf::Vector2i pos, std::vector<sf::Vector2i> &ans);

public:
	Board(int x, int y);

	void erase(sf::Vector2i pos);
	void clearCells();
	void setFont(sf::Font &newFont);
	void resetPosition(int x, int y);
	void setName(const std::string &newName) { name = newName; };
	void draw(sf::RenderWindow &window) const;
	void updateCells();
	void showAlive();
	void DrawShipBorders(sf::RenderWindow &wnd);

	int hit(sf::Vector2i &position);
	int getShipNum();
	bool hasLost();
	bool addShip(sf::Vector2i begin, sf::Vector2i end);
	void RandomFill();
	void DeleteShips();

	bool fromHash(const std::string &s);

	std::string getName();
	std::string hash() const;
	std::vector<int> &getCountShips();
	std::vector<sf::RectangleShape> &operator[](int index);

	sf::RectangleShape &operator[](sf::Vector2i pos);
};