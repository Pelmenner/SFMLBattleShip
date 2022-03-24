#pragma once

#include "Colors.h"

#include <SFML/Graphics.hpp>
#include <vector>

class Field : public sf::Drawable
{
public:
    EnemyBoard(sf::Vector2i position, const sf::Font& fnt);

    void setName(const std::string& name);
    void setPosition(sf::Vector2i position);

    bool hasLost() const;
    std::string getName() const;
    sf::RectangleShape& operator[](const sf::Vector2i& pos);
    std::vector<sf::RectangleShape>& operator[](int i);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    int surroundDestroyed(sf::Vector2i pos);
    static void getNeighbours(sf::Vector2i pos, std::vector<sf::Vector2i>& res);
};