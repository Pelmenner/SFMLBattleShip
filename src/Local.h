#pragma once

#include "LocalField.h"
#include "Game.h"

#include <SFML/Graphics.hpp>

class Local : public Game
{
public:
	Local(sf::RenderWindow& wnd);

	void setNames(const std::string& name1, const std::string& name2);
	void initPlayers();

	bool play() override;

	std::pair<Field*, Field*> getFields() const override;

private:
	int gameMode = 0;
	LocalField* field1, * field2;
};
