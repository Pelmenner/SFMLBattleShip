#pragma once

#include "Field.h"
#include <array>

class LocalField : public Field
{
public:
    LocalField(sf::Vector2i pos, const sf::Font& fnt);

    void erase(sf::Vector2i pos);
    void showAllShips();
    void showRemainingShips();
    void showShipBorders();

    int hit(sf::Vector2i pos);
    int getTotalShips();
    bool addShip(sf::Vector2i begin, sf::Vector2i end);
    void randomFill();
    void removeShips();

    const std::array<int, 5> getShipCount() const;
};
