#pragma once

#include "Field.h"

class RemoteField : public Field
{
public:
    RemoteField(sf::Vector2i pos, const sf::Font& fnt);

    void setHit(sf::Vector2i pos, int hit);
};