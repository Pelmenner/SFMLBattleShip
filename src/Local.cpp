#include "Functions.h"
#include "Resources.h"
#include "Button.h"
#include "Local.h"

Local::Local(sf::RenderWindow& wnd) :
    Game(wnd),
    field1(new LocalField({ 50, 100 })), 
    field2(new LocalField({ 50, 100 }))
{
}

bool Local::play()
{
    const sf::Vector2f buttonSize = sf::Vector2f(200.0f, 71.0f);

    Button replayButton({ 800.0f, 670.0f }, buttonSize, "Replay");
    Button exitButton({ 1100.0f, 670.0f }, buttonSize, "Exit");

    std::string add = "'s turn";
    sf::Text turn;
    setText(turn, { 20.0f, 680.0f }, nameColor, *resources.titleFont, field1->getName() + add);

    sf::Text arrow;
    arrow.setOrigin(17.5, 17.5f);
    setText(arrow, { 600.0f, 350.0f }, nameColor, *resources.titleFont, "=>");

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

        window.clear(blackColor);

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

void Local::setNames(const std::string& name1, const std::string& name2)
{
    field1->setName(name1), field2->setName(name2);
}

void Local::initPlayers()
{
    initPlayer(field1);
    initPlayer(field2);
}
