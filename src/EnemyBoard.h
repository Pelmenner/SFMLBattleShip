#pragma once

#include "Colors.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <random>
#include <ctime>

class EnemyBoard
{
private:
	std::string name;
	std::vector <std::vector <sf::RectangleShape>> cells;
	sf::Text letters[10] = {};
	sf::Text numbers[10] = {};
	sf::Font font;
	std::vector <int> ships_remaining;

	void GetNeighbours(sf::Vector2i pos, std::vector <sf::Vector2i> &ans);
	void SurroundKilled(sf::Vector2i pos, sf::Vector2i prev, int &len);

public:
	EnemyBoard(int x, int y);

	void Draw(sf::RenderWindow &wnd) const;
	void SetFont(sf::Font fnt);
	bool HasLost() const;
	void SetHit(sf::Vector2i pos, int hit);
	void SetName(const std::string &name);
	void ResetPosition(int x, int y);

	std::string GetName() const;
	sf::RectangleShape &operator[](const sf::Vector2i &pos);
	std::vector <sf::RectangleShape> &operator[](int x);
};

