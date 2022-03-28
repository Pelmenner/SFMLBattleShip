#pragma once

#include "Field.h"
#include "RandomGenerator.h"
#include <array>

class LocalField : public Field
{
public:
    LocalField(sf::Vector2f pos, const sf::Font& fnt);

    void erase(sf::Vector2i pos);
    void showRemainingShips();
    void showShipBorders();

    int hit(sf::Vector2i pos);
    bool addShip(sf::Vector2i begin, sf::Vector2i end);
    void randomFill();
    void removeShips();

    int countShips() const;
    int countShips(int length) const;

private:
    struct Ship
    {
        int length;
        int health;
        sf::Vector2i position;
        sf::Vector2i direction;

        Ship();
        Ship(sf::Vector2i start, sf::Vector2i end);

        bool contains(sf::Vector2i pos) const;
    };

    std::vector<Ship> ships;

    void revealPlacement();
    bool checkShipPosition(const Ship& ship) const;
};
