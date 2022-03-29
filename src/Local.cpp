#include "Functions.h"
#include "Button.h"
#include "Local.h"

Local::Local(sf::RenderWindow& wnd, sf::Font& titleFont, sf::Font& fnt) :
    Game(wnd, titleFont, fnt),
    field1(new LocalField({ 50, 100 }, fnt)), 
    field2(new LocalField({ 50, 100 }, fnt))
{
}

bool Local::play()
{
    const sf::Vector2f buttonSize = sf::Vector2f(122.0f, 71.0f);

    sf::Font buttonFont;
    buttonFont.loadFromFile("TitleFont.ttf");

    Button replayButton({ 800.0f, 630.0f }, buttonSize, buttonFont, "Replay");
    Button exitButton({ 1000.0f, 630.0f }, buttonSize, buttonFont, "Exit");

    std::string add = "'s turn";
    sf::Text turn;
    setText(turn, { 20.0f, 680.0f }, nameColor, titleFont, field1->getName() + add);

    sf::Text name1(field1->getName(), titleFont);
    sf::Text name2(field2->getName(), titleFont);
    name1.setFillColor(nameColor);
    name2.setFillColor(nameColor);
    name1.setPosition(300, 10);
    name2.setPosition(900, 10);

    sf::Text arrow;
    arrow.setOrigin(17.5, 17.5f);
    setText(arrow, { 600.0f, 350.0f }, nameColor, titleFont, "=>");

    int shot = 0;
    bool finished = false;

    field2->setPosition({ 700, 100 });
    field1->clearCells();
    field2->clearCells();

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event evnt;
        while (window.pollEvent(evnt))
        {
            switch (evnt.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::MouseButtonPressed:
                sf::Vector2f pos_float = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                sf::Vector2i pos(pos_float);

                if (finished)
                {
                    if (replayButton.contains(pos_float) && evnt.mouseButton.button == sf::Mouse::Left)
                        return true;
                    if (exitButton.contains(pos_float) && evnt.mouseButton.button == sf::Mouse::Left)
                        return false;
                    break;
                }

                if (shot % 2 == 1 && (pos.x > 550 || pos.x < 50))
                    break;
                if (shot % 2 == 0 && (pos.x < 690 || pos.x > 1190))
                    break;
                if (pos.y < 100 || pos.y > 600)
                    break;

                if (shot % 2 == 1)
                {
                    pos.x = (pos.x - 50) / 50;
                    pos.y = (pos.y - 100) / 50;

                    if ((*field1)[pos].getFillColor() != blackColor) break;

                    if (field1->hit(pos) == 0)
                    {
                        (*field1)[pos].setFillColor(grayColor);
                        shot++;
                        arrow.rotate(180.0f);
                        turn.setString(field2->getName() + add);
                    }
                    else
                        break;
                }
                else
                {
                    pos.x = (pos.x - 690) / 50;
                    pos.y = (pos.y - 100) / 50;

                    if ((*field2)[pos].getFillColor() != blackColor) break;

                    if (field2->hit(pos) == 0)
                    {
                        (*field2)[pos].setFillColor(grayColor);
                        shot++;
                        arrow.rotate(180.0f);
                        turn.setString(field1->getName() + add);
                    }
                    else break;
                }
            }
        }

        if (!finished && field1->hasLost())
        {
            finished = true;
            turn.setString(field2->getName() + " won!");
            field2->showRemainingShips();
        }
        if (!finished && field2->hasLost())
        {
            finished = true;
            turn.setString(field1->getName() + " won!");
            field1->showRemainingShips();
        }

        window.clear();

        window.draw(background);
        window.draw(name1);
        window.draw(name2);
        window.draw(turn);

        window.draw(*field1);
        window.draw(*field2);
        window.draw(arrow);

        if (finished)
        {
            window.draw(replayButton);
            window.draw(exitButton);
        }

        window.display();

        sf::sleep(sf::milliseconds(20 - clock.getElapsedTime().asMilliseconds()));
        clock.restart();
    }

    return false;
}

std::pair<Field*, Field*> Local::getFields() const
{
    return { field1, field2 };
}

void Local::setBlocks(std::vector<sf::RectangleShape>& blocks)
{
    for (int i = 0; i < 10; ++i)
    {
        blocks[i].setFillColor(blackColor);
        blocks[i].setOutlineColor(sf::Color::White);
        blocks[i].setOutlineThickness(5);
        blocks[i].setSize({ 50, 50 });
    }

    for (int i = 0; i < 4; ++i)
        for (int j = 0; j <= i; ++j)
            blocks[(i * (i + 1)) / 2 + j].setPosition(775.0f + (float)j * 50.0f, 100.0f + (float)i * 100.0f);
}

void Local::setNames(const std::string& name1, const std::string& name2)
{
    field1->setName(name1), field2->setName(name2);
}

void Local::initPlayers()
{
    initPlayer(field1);
    initPlayer(field2);
}