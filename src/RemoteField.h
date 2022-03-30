#pragma once

#include "Field.h"

class RemoteField : public Field
{
public:
    RemoteField(sf::Vector2f pos);

    void setHit(sf::Vector2i pos, int hit);
};