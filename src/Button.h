#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Button
{
public:
    Button(const std::string& texturePath = "data/button.png");
    Button(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Font& font, 
        const std::string& text = "", const std::string & texturePath = "data/button.png");
    ~Button();

    bool contains(const sf::Vector2f& mousePos);
    void setText(const std::string& newText);
    void draw(sf::RenderWindow& window) const;
    void setPosition(const sf::Vector2f position);
    void setSize(const sf::Vector2f size);

private:
    sf::Texture* texture;
    sf::RectangleShape shape;
    sf::Text text;

    void updateTextPosition();
};