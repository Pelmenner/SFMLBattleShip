#include "LocalField.h"

LocalField::LocalField(sf::Vector2f pos) : Field(pos)
{
    shipsCount.fill(0);
}

void LocalField::erase(sf::Vector2i pos)
{
    auto it = std::find_if(ships.begin(), ships.end(), [&pos](const Ship& ship) {
        return ship.contains(pos);
    });
    if (it == ships.end())
        return;

    --shipsCount[it->length];
    ships.erase(it);
    revealPlacement();
}

void LocalField::showRemainingShips()
{
    for (const Ship& ship : ships)
    {
        sf::Vector2i end = ship.position + ship.direction * ship.length;
        for (sf::Vector2i pos = ship.position; pos != end; pos += ship.direction)
            if (operator[](pos).getFillColor() == grayColor)
                operator[](pos).setFillColor(greenColor);
    }
}

void LocalField::showShipBorders()
{
    for (const Ship& ship : ships)
    {
        sf::Vector2i end = ship.position + ship.direction * ship.length;
        for (sf::Vector2i pos = ship.position; pos != end; pos += ship.direction)
            operator[](pos).setOutlineColor(redColor);
    }
}

int LocalField::hit(sf::Vector2i pos)
{
    auto it = std::find_if(ships.begin(), ships.end(), [&pos](const Ship& ship) {
        return ship.contains(pos);
    });

    int status;
    if (it == ships.end())
    {
        status = 0;
    }
    else
    {
        Ship& ship = *it;
        --ship.health;
        status = 1;
        operator[](pos).setFillColor(redColor);

        if (ship.health == 0)
        {
            status = 2;
            --shipsCount[ship.length];
            surroundDestroyed(ship.position);
        }
    }

    return status;
}

bool LocalField::addShip(sf::Vector2i begin, sf::Vector2i end)
{
    if (begin.x > end.x || begin.y > end.y)
        std::swap(begin, end);
    if (begin.x != end.x && begin.y != end.y)
        return false;

    Ship ship(begin, end);
    end = begin + ship.direction * ship.length;
    if (ship.length < 1 || ship.length > 4)
        return false;
    if (shipsCount[ship.length] == 5 - ship.length)
        return false;
    if (!checkShipPosition(ship))
        return false;

    ships.push_back(ship);
    ++shipsCount[ship.length];
    revealPlacement();
    return true;
}

void LocalField::randomFill()
{
    removeShips();

    RandomGenerator generator;
    for (int length = 4; length >= 1; --length)
    {
        const int count = 5 - length;
        shipsCount[length] = count;
        for (int i = 0; i < count; ++i)
        {
            Ship ship;
            ship.length = ship.health = length;
            do
            {
                ship.position.x = generator.randInt(0, 9);
                ship.position.y = generator.randInt(0, 10 - length);
                ship.direction.x = generator.randInt(0, 1);
                ship.direction.y = 1 - ship.direction.x;
                if (ship.direction.x == 1)
                    std::swap(ship.position.x, ship.position.y);

            } while (!checkShipPosition(ship));

            ships.push_back(ship);
        }
    }

    revealPlacement();
}

void LocalField::removeShips()
{
    clearCells();
    ships.clear();
    shipsCount.fill(0);
}

int LocalField::countShips() const
{
    return std::accumulate(std::begin(shipsCount), std::end(shipsCount), 0);
}

int LocalField::countShips(int length) const
{
    if (length < 1 || length > 4)
        return 0;
    return shipsCount[length];
}

LocalField::Ship::Ship() : length(), health(), position(), direction() {};

LocalField::Ship::Ship(sf::Vector2i start, sf::Vector2i end)
{
    length = (end.x - start.x) + (end.y - start.y) + 1;
    health = length;
    position = start;
    direction = end - start;
    if (length > 1)
        direction /= length - 1;
    else
        direction = { 1, 0 };
}

bool LocalField::Ship::contains(sf::Vector2i pos) const
{
    sf::Vector2i diff = pos - position;
    if (direction.x == 0 && diff.x != 0)
        return false;
    if (direction.y == 0 && diff.y != 0)
        return false;
    if (direction.x != 0 && (diff.x < 0 || diff.x >= length))
        return false;
    if (direction.y != 0 && (diff.y < 0 || diff.y >= length))
        return false;
    return true;
}

void LocalField::revealPlacement()
{
    clearCells();
    for (const Ship& ship : ships)
    {
        std::vector<sf::Vector2i> adjacent;
        const sf::Vector2i end = ship.position + ship.direction * ship.length;

        for (sf::Vector2i pos = ship.position; pos != end; pos += ship.direction)
            getNeighbours(pos, adjacent);
        for (const sf::Vector2i& pos : adjacent)
            operator[](pos).setFillColor(grayColor);
        for (sf::Vector2i pos = ship.position; pos != end; pos += ship.direction)
            operator[](pos).setFillColor(redColor);
    }
}

bool LocalField::checkShipPosition(const Ship& ship) const
{
    std::vector<sf::Vector2i> requiredCells;
    const sf::Vector2i end = ship.position + ship.direction * ship.length;
    for (sf::Vector2i pos = ship.position; pos != end; pos += ship.direction)
    {
        getNeighbours(pos, requiredCells);
        requiredCells.push_back(pos);
    }
    for (const sf::Vector2i& pos : requiredCells)
        for (const Ship& placedShip : ships)
            if (placedShip.contains(pos))
                return false;
    return true;
}
