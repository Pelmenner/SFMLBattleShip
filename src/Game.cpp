#include "Connection.h"
#include "Functions.h"
#include "Game.h"

Game::Game(sf::RenderWindow& wnd, sf::Font& titleFont, sf::Font& fnt) :
    window(wnd), titleFont(titleFont), font(fnt)
{
}

void Game::setShipNum(std::vector<sf::Text>& shipNum, sf::Font& fnt)
{
    for (size_t i = 0; i < shipNum.size(); ++i)
    {
        shipNum[i].setFont(fnt);
        shipNum[i].setFillColor(sf::Color::White);
        shipNum[i].setPosition(700.0f, 100.0f + (float)i * 100.0f);
        shipNum[i].setString(std::to_string(4 - i) + " x ");
    }
}

void Game::setBlocks(std::vector<sf::RectangleShape>& blocks)
{
    for (size_t i = 0; i < 10; ++i)
    {
        blocks[i].setFillColor(blackColor);
        blocks[i].setOutlineColor(sf::Color::White);
        blocks[i].setOutlineThickness(5);
        blocks[i].setSize({ 50, 50 });
    }

    for (size_t i = 0; i < 4; ++i)
        for (size_t j = 0; j <= i; ++j)
            blocks[(i * (i + 1)) / 2 + j].setPosition(775.0f + j * 50.0f, 100.0f + i * 100.0f);
}

void Game::initPlayer(LocalField* field)
{
    const sf::Vector2f buttonSize = sf::Vector2f(200.0f, 71.0f);
    sf::Font buttonFont;
    buttonFont.loadFromFile("data/TitleFont.ttf");

    Initializer initializer(field, buttonSize, buttonFont);

    sf::Clock clock;
    while (window.isOpen() && !initializer.isReady())
    {
        sf::Event evnt;
        while (window.pollEvent(evnt))
            initializer.processEvent(evnt, window);

        initializer.resetShipNum();

        window.clear(blackColor);
        window.draw(initializer);
        window.display();

        sf::sleep(sf::milliseconds(40 - clock.getElapsedTime().asMilliseconds()));
        clock.restart();
    }
}

void Game::Initializer::processEvent(sf::Event& event, sf::RenderWindow& window)
{
    switch (event.type)
    {
    case sf::Event::Closed:
        window.close();
        break;
    case sf::Event::KeyPressed:
        if (event.key.code == sf::Keyboard::Space)
        {
            if (hider.getFillColor() == sf::Color::Transparent)
                hider.setFillColor(blackColor);
            else
                hider.setFillColor(sf::Color::Transparent);
        }
        break;
    case sf::Event::MouseButtonPressed:
        if (hider.getFillColor() == blackColor)
            return;
        sf::Vector2f posFloat = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::Vector2i pos(posFloat);

        if (event.mouseButton.button == sf::Mouse::Left)
        {
            if (randomButton.contains(posFloat))
            {
                drawing = false;
                field->randomFill();
                break;
            }
            if (field->countShips() == 10 && acceptButton.contains(posFloat))
            {
                ready = true;
                return;
            }
            if (clearButton.contains(posFloat))
            {
                drawing = false;
                field->removeShips();
                break;
            }
        }

        pos.x = (pos.x - 50) / 50;
        pos.y = (pos.y - 100) / 50;

        if (pos.x < 0 || pos.x >= 10 || pos.y < 0 || pos.y >= 10)
            break;

        if (event.mouseButton.button == sf::Mouse::Left)
        {
            if (!drawing)
            {
                if ((*field)[pos].getFillColor() == blackColor)
                {
                    (*field)[pos].setFillColor(yellowColor);
                    drawing = true;
                }

                buf_pos = pos;
            }
            else
            {
                if (!field->addShip(buf_pos, pos))
                    (*field)[buf_pos].setFillColor(blackColor);
                drawing = false;
            }
        }
        else if (event.mouseButton.button == sf::Mouse::Right)
        {
            if (drawing)
                (*field)[buf_pos].setFillColor(blackColor);
            drawing = false;
            field->erase(pos);
        }
    }
}

void Game::Initializer::resetShipNum()
{
    for (size_t i = 0; i < shipNum.size(); ++i)
        shipNum[i].setString(std::to_string(4 - i - field->countShips(i + 1)) + " x ");
}

Game::Initializer::Initializer(LocalField* field, sf::Vector2f buttonSize, sf::Font& buttonFont) :
    field(field),
    randomButton({ 700.0f, 500.0f }, buttonSize, buttonFont, "Random"),
    clearButton({ 1000.0f, 500.0f }, buttonSize, buttonFont, "Clear"),
    acceptButton({ 850.0f, 600.0f }, buttonSize, buttonFont, "Accept"),
    drawing(false), ready(false), buf_pos(0, 0), shipNum(4), blocks(10), hider({ 1280, 670 })
{
    sf::Font& titleFont = buttonFont;
    name = sf::Text(field->getName(), titleFont);
    name.setPosition(100, 10);
    name.setFillColor(nameColor);

    setShipNum(shipNum, titleFont);
    setBlocks(blocks);

    hider.setFillColor(sf::Color::Transparent);
    hider.setPosition(0, 50);
}

bool Game::Initializer::isReady() const
{
    return ready;
}

void Game::Initializer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(name, states);

    if (field->countShips() == 10)
        target.draw(acceptButton, states);

    target.draw(randomButton, states);
    target.draw(clearButton, states);

    for (int i = 0; i < 10; ++i)
        target.draw(blocks[i], states);
    for (int i = 0; i < 4; ++i)
        target.draw(shipNum[i], states);

    target.draw(*field, states);
    target.draw(hider, states);
}
