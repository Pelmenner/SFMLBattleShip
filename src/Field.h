#pragma once

#include "Colors.h"

#include <SFML/Graphics.hpp>

#include <numeric>
#include <string>
#include <vector>
#include <array>

class Field : public sf::Drawable
{
public:
    Field(sf::Vector2f pos, const sf::Font& fnt);

    void setName(const std::string& name);
    void setPosition(sf::Vector2f pos);
    void clearCells();

    bool hasLost() const;
    const std::string& getName() const;
    sf::RectangleShape& operator[](sf::Vector2i pos);
    std::vector<sf::RectangleShape>& operator[](int i);

protected:
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    int surroundDestroyed(sf::Vector2i pos);
    static void getNeighbours(sf::Vector2i pos, std::vector<sf::Vector2i>& res);
    std::array<int, 5> shipsCount;

private:
    sf::Vector2f position;
    sf::Text name;
    std::vector<std::vector<sf::RectangleShape>> cells;
    std::array<sf::Text, 10> letters;
    std::array<sf::Text, 10> numbers;
    const sf::Font& font;

    void dfs(sf::Vector2i pos, sf::Vector2i prev, int& len);
    void updateLayout();
};