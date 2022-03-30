#pragma once

#include <SFML/Graphics.hpp>

namespace resources
{
    inline sf::Font titleFont;
    inline sf::Font mainFont;
    inline sf::Texture* buttonTexture;

    inline void loadResources()
    {
        mainFont.loadFromFile("data/font.ttf");
        titleFont.loadFromFile("data/TitleFont.ttf");
        buttonTexture = new sf::Texture();
        buttonTexture->loadFromFile("data/button.png");
        buttonTexture->setSmooth(true);
    }

    inline void freeResources()
    {
        mainFont.loadFromFile("");
        titleFont.loadFromFile("");
        delete buttonTexture;
    }
};