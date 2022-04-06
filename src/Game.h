#pragma once

#include "Connection.h"
#include "LocalField.h"
#include "Button.h"
#include "Field.h"

#include <SFML/Graphics.hpp>

class Game
{
public:
	Game(sf::RenderWindow &wnd);

	virtual bool play() = 0;
	virtual std::pair<Field*, Field*> getFields() const = 0;

protected:
	static void setShipNum(std::vector<sf::Text> &shipNum);
	static void setBlocks(std::vector<sf::RectangleShape> &blocks);
	void initPlayer(LocalField *field);
	
	sf::RenderWindow& window;

private:
	class Initializer : public sf::Drawable
	{
	public:
		Initializer(LocalField* board, sf::Vector2f buttonSize);
		void processEvent(sf::Event& event, sf::RenderWindow& window);
		void resetShipNum();
		bool isReady() const;

	protected:
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	private:
		LocalField* field;

        Button randomButton;
        Button clearButton;
        Button acceptButton;

        bool drawing;
		bool ready;
        sf::Vector2i buf_pos;

        std::vector<sf::Text> shipNum;
        std::vector<sf::RectangleShape> blocks;

        sf::RectangleShape hider;
	};
};
