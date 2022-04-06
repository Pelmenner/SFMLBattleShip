#pragma once

#include "Connection.h"
#include "LocalField.h"
#include "RemoteField.h"
#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <string>

class Online : public Game
{
public:
    Online(sf::RenderWindow& wnd, Connection* mult);

    void setCells(std::vector <sf::RectangleShape>& cells);

    void initLocal(const std::string& name);
    void setOpponentName(const std::string& name);
    void setLocalMove(int localMove);

    std::pair<Field*, Field*> getFields() const override;
    bool play() override;

private:
    LocalField* local;
    RemoteField* opponent;
    Connection* mult;
    int localMove;
};
