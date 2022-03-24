#include "RemoteField.h"

RemoteField::RemoteField(sf::Vector2f pos, const sf::Font& fnt) : Field(pos, fnt)
{
}

void RemoteField::setHit(sf::Vector2i pos, int hit)
{
    switch (hit)
    {
    case 0:
        operator[](pos).setFillColor(grayColor);
        break;
    case 1:
        operator[](pos).setFillColor(redColor);
        break;
    case 2:
        operator[](pos).setFillColor(redColor);
        int length = surroundDestroyed(pos);
        --shipsCount[length];
        break;
    }
}