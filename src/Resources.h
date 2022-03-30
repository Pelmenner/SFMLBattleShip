#pragma once

#include <SFML/Graphics.hpp>

class Resources
{
public:
    sf::Font* titleFont;
    sf::Font* mainFont;
    sf::Texture* buttonTexture;

    void loadResources()
    {
        titleFont = new sf::Font();
        mainFont = new sf::Font();
        mainFont->loadFromFile("data/font.ttf");
        titleFont->loadFromFile("data/TitleFont.ttf");
        buttonTexture = new sf::Texture();
        buttonTexture->loadFromFile("data/button.png");
        buttonTexture->setSmooth(true);
    }

    void freeResources()
    {
        delete mainFont;
        delete titleFont;
        delete buttonTexture;
    }
};

inline Resources resources;