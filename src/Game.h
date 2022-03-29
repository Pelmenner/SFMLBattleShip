#pragma once

#include "Connection.h"
#include "LocalField.h"
#include "Button.h"
#include "Field.h"

#include <SFML/Graphics.hpp>

class Game
{
public:
	Game(sf::RenderWindow &wnd, sf::Font &titleFont, sf::Font &fnt);

	virtual bool play() = 0;
	virtual void initPlayers() = 0;
	virtual std::pair<Field*, Field*> getFields() const = 0;

protected:
	static void setShipNum(std::vector<sf::Text> &shipNum, sf::Font &fnt);
	static void setBlocks(std::vector<sf::RectangleShape> &blocks);
	void initPlayer(LocalField *field);
	
	sf::RenderWindow& window;
	sf::Font& titleFont;
	sf::Font& font;

private:
	class Initializer : public sf::Drawable
	{
	public:
		Initializer(LocalField* board, sf::Vector2f buttonSize, sf::Font& buttonFont);
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

        sf::Text name;
        bool drawing;
		bool ready;
        sf::Vector2i buf_pos;

        std::vector<sf::Text> shipNum;
        std::vector<sf::RectangleShape> blocks;

        sf::RectangleShape hider;
	};
};
