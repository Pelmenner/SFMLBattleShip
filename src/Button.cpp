#include "Button.h"
#include "Colors.h"

Button::Button(const std::string& texturePath): texture(new sf::Texture())
{
    texture->loadFromFile(texturePath);
    texture->setSmooth(true);
    shape.setTexture(texture);
}

Button::Button(const sf::Vector2f& position, const sf::Vector2f& size, 
    const sf::Font& font, const std::string& buttonText, const std::string& texturePath):
    texture(new sf::Texture())
{
    texture->loadFromFile(texturePath);
    texture->setSmooth(true);

    shape.setSize(size);
    shape.setPosition(position.x - size.x / 2.0f, position.y - size.y / 2.0f);
    shape.setTexture(texture);

    text.setFont(font);
    text.setCharacterSize(40);
    text.setFillColor(whiteColor);
    text.setStyle(sf::Text::Bold);
    text.setString(buttonText);
    updateTextPosition();
}

Button::~Button()
{
    delete texture;
}

bool Button::contains(const sf::Vector2f& mousePos)
{
    return shape.getGlobalBounds().contains(mousePos);
}

void Button::setText(const std::string& newText)
{
    text.setString(newText);
    updateTextPosition();
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(shape, states);
    target.draw(text, states);
}

void Button::updateTextPosition()
{
    text.setPosition(shape.getGlobalBounds().left + (shape.getSize().x - text.getGlobalBounds().width) / 2.0f,
        shape.getGlobalBounds().top + (shape.getSize().y - text.getGlobalBounds().height) / 2.0f - 15);
}

void Button::setPosition(const sf::Vector2f position)
{
    shape.setPosition(position.x - shape.getSize().x, position.y - shape.getSize().y);
    updateTextPosition();
}

void Button::setSize(const sf::Vector2f size)
{
    shape.setSize(size);
    updateTextPosition();
}
