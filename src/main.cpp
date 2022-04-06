#include "Connection.h"
#include "Functions.h"
#include "Resources.h"
#include "Button.h"
#include "Online.h"
#include "Local.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <string>
#include <vector>

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

using std::vector;
using sf::Vector2i;
using sf::RenderWindow;
using std::string;
using std::pair;

bool enterName(RenderWindow& wnd, string text, string& result);

int menu(sf::RenderWindow& window);

int selectConnectionType(sf::RenderWindow& wnd);

int waitConnection(sf::RenderWindow& wnd, Connection& mult);

int waitOpponent(sf::RenderWindow& wnd, Connection& mult, std::string& opponentName);

int main()
{
    ResourceWrapper resourceWrapper;

    sf::Vector2i wndSize = getWindowSize();

    RenderWindow wnd(sf::VideoMode(wndSize.x, wndSize.y), "Sea Battle");
    wnd.setFramerateLimit(60);
    wnd.setKeyRepeatEnabled(false);

    sf::View view;
    view.reset(sf::FloatRect(0.0f, 0.0f, 1280.0f, 720.0f));
    view.setViewport(sf::FloatRect(0.0f, 0.0f, 1.0f, 1.0f));
    wnd.setView(view);

    sf::Image icon;
    icon.loadFromFile("data/icon.png");
    wnd.setIcon(30, 30, icon.getPixelsPtr());

    int choice = -1;

    while (choice != 0)
    {
        choice = menu(wnd);

        if (choice == 1)
        {
            string name1, name2;
            if (!enterName(wnd, "Enter first player's name: ", name1))
                break;

            if (!enterName(wnd, "Enter second player's name: ", name2))
                break;

            bool res = true;
            while (res)
            {
                Local game(wnd);
                game.setNames(name1, name2);

                game.initPlayers();

                res = game.play();
            }
        }
        else if (choice == 2)
        {
            std::string nickname;
            if (!enterName(wnd, "Enter your multiplayer nickname: ", nickname))
                break;

            bool ret = true;
            while (ret)
            {
                int type = selectConnectionType(wnd);
                if (type == 0)
                    return 0;
                if (type == 4)
                    break;

                Connection session;

                if (type == 1)
                {
                    int res = waitConnection(wnd, session);
                    if (res == 0)
                        return 0;
                    if (res == 1)
                        continue;

                    Online game(wnd, &session);
                    game.initLocal(nickname);
                    session.sendName(nickname);
                    std::string opponent = "";
                    res = waitOpponent(wnd, session, opponent);
                    if (res == 0)
                        return 0;
                    game.setOpponentName(opponent);

                    game.setLocalMove(0);
                    ret = game.play();

                    session.disconnect();
                }
                else if (type == 2)
                {
                    std::string ip;
                    if (!enterName(wnd, "Enter server ip address: ", ip))
                        break;

                    if (!session.activeConnection(ip))
                        continue;

                    Online game(wnd, &session);
                    game.initLocal(nickname);
                    session.sendName(nickname);
                    std::string opponent = "";
                    int res = waitOpponent(wnd, session, opponent);
                    if (res == 0)
                        return 0;
                    game.setOpponentName(opponent);

                    game.setLocalMove(1);
                    ret = game.play();

                    session.disconnect();
                }
            }
        }
    }

    if (wnd.isOpen())
        wnd.close();

    return 0;
}

int waitConnection(sf::RenderWindow& wnd, Connection& mult)
{
    sf::Text txt;
    txt.setString("Waiting for connection...");
    txt.setFont(*resources.mainFont);
    txt.setFillColor(grayColor);
    txt.setCharacterSize(80);
    txt.setOrigin(txt.getLocalBounds().width / 2, txt.getLocalBounds().height / 2);
    txt.setPosition(1280.f / 2, 720.f / 2);

    while (wnd.isOpen())
    {
        sf::Event evnt;
        while (wnd.pollEvent(evnt))
        {
            switch (evnt.type)
            {
            case sf::Event::Closed:
                wnd.close();
                return 0;
            case sf::Event::KeyPressed:
                if (evnt.key.code == sf::Keyboard::Escape)
                {
                    mult.disconnect();
                    return 1;
                }
            }
        }

        if (mult.passiveConnection())
            break;

        wnd.clear(blackColor);
        wnd.draw(txt);
        wnd.display();

        sf::sleep(sf::milliseconds(40));
    }

    return 2;
}

int waitOpponent(sf::RenderWindow& wnd, Connection& mult, std::string& opponentName)
{
    sf::Text txt;
    txt.setString("Waiting for other player...");
    txt.setFont(*resources.mainFont);
    txt.setFillColor(grayColor);
    txt.setCharacterSize(80);
    txt.setOrigin(txt.getLocalBounds().width / 2, txt.getLocalBounds().height / 2);
    txt.setPosition(1280.f / 2, 720.f / 2);

    while (wnd.isOpen() && !mult.receiveName(opponentName))
    {
        sf::Event evnt;
        while (wnd.pollEvent(evnt))
        {
            if (evnt.type == sf::Event::Closed)
            {
                wnd.close();
                return 0;
            }
        }

        wnd.clear(blackColor);
        wnd.draw(txt);
        wnd.display();

        sf::sleep(sf::milliseconds(40));
    }

    return 1;
}

int selectConnectionType(sf::RenderWindow& wnd)
{
    const sf::Vector2f buttonSize(800.f, 100.f);

    Button hostButton({ 1280.f / 2, 250.f }, buttonSize, "Host game");
    Button joinButton({ 1280.f / 2, 400.f }, buttonSize, "Join game");
    Button serverButton({ 1280.f / 2, 550.f }, buttonSize, "Connect to server");

    sf::Text title;
    title.setFont(*resources.titleFont);
    title.setString("Choose multiplayer connection type: ");
    title.setFillColor(whiteColor);
    title.setCharacterSize(36);
    title.setOrigin(title.getLocalBounds().width / 2, 0.f);
    title.setPosition(1280.f / 2, 50.f);
    title.setStyle(sf::Text::Bold);

    while (wnd.isOpen())
    {
        sf::Event evnt;
        while (wnd.pollEvent(evnt))
        {
            switch (evnt.type)
            {
            case sf::Event::Closed:
                wnd.close();
                return 0;
            case sf::Event::KeyPressed:
                if (evnt.key.code == sf::Keyboard::Escape)
                    return 4;
                break;
            case sf::Event::MouseButtonPressed:
                if (evnt.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f pos = wnd.mapPixelToCoords(sf::Mouse::getPosition(wnd));
                    if (hostButton.contains(pos))
                        return 1;
                    if (joinButton.contains(pos))
                        return 2;
                }
            }
        }

        wnd.clear(blackColor);

        wnd.draw(title);

        wnd.draw(hostButton);
        wnd.draw(joinButton);
        wnd.draw(serverButton);

        wnd.display();

        sf::sleep(sf::milliseconds(40));
    }
    return -1;
}

bool enterName(RenderWindow& wnd, string text, string& result)
{
    sf::RectangleShape space;
    space.setSize({ 500, 80 });
    space.setOutlineColor(blueColor);
    space.setOutlineThickness(5);
    space.setFillColor(grayColor);
    space.setOrigin(space.getLocalBounds().width / 2, space.getLocalBounds().height / 2);
    space.setPosition(1280.f / 2, 720 / 2.0f * 0.8f);

    sf::Text title;
    title.setFont(*resources.mainFont);
    title.setString(text);
    title.setFillColor(sf::Color::White);
    title.setCharacterSize(1280 / 20);
    title.setOrigin(title.getLocalBounds().width / 2, 0);
    title.setPosition(1280.f / 2, 720.f / 5);

    Button okButton({ 1280.f / 2, 720.f / 2 * 1.4f }, { 300, 120 });
    okButton.setText("OK");

    sf::Text txt;
    txt.setFont(*resources.mainFont);
    txt.setFillColor(txtColor);
    txt.setPosition(space.getPosition());
    const float originY = 25.f;
    txt.setOrigin(txt.getLocalBounds().width / 2, originY);

    sf::Clock clock;
    while (wnd.isOpen())
    {
        sf::Event evnt;
        while (wnd.pollEvent(evnt))
        {
            switch (evnt.type)
            {
            case sf::Event::Closed:
                wnd.close();
                break;

            case sf::Event::MouseButtonPressed:
                if (evnt.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f pos = wnd.mapPixelToCoords(sf::Mouse::getPosition(wnd));
                    if (okButton.contains({ pos.x, pos.y }))
                        return true;
                }
                break;

            case sf::Event::KeyPressed:
                if (evnt.key.code == sf::Keyboard::Escape)
                    return false;
                break;

            case sf::Event::TextEntered:
                switch (evnt.text.unicode)
                {
                case 0xD: //Return
                    return true;
                case 0x8: //Backspace
                    if (!result.empty())
                        result.pop_back();
                    break;
                default:
                    result += (wchar_t)evnt.text.unicode;
                }
                txt.setString(result);
                if (txt.getLocalBounds().width > space.getLocalBounds().width && !result.empty())
                {
                    result.pop_back();
                    txt.setString(result);
                }
                txt.setOrigin(txt.getLocalBounds().width / 2, originY);
            }
        }

        wnd.clear(blackColor);
        wnd.draw(space);
        wnd.draw(txt);
        wnd.draw(title);
        wnd.draw(okButton);
        wnd.display();

        sf::sleep(sf::milliseconds(20 - clock.getElapsedTime().asMilliseconds()));
        clock.restart();
    }

    return true;
}

int menu(sf::RenderWindow& window)
{
    sf::RectangleShape bg;
    bg.setFillColor(blackColor);
    bg.setSize(sf::Vector2f(1280.0f, 720.0f));

    const sf::Vector2f buttonSize = sf::Vector2f(244, 142);
    Button localButton({ 1280 / 3.0f, 720 / 3.0f }, buttonSize);
    Button onlineButton({ 1280 / 3.0f * 2.0f, 720 / 3.0f }, buttonSize);
    Button exitButton({ 1280 / 2.0f, 720 / 3.0f * 2.0f }, buttonSize);

    localButton.setText("Local");
    onlineButton.setText("Online");
    exitButton.setText("Exit");

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
                return 0;

            case sf::Event::KeyPressed:
                if (evnt.key.code == sf::Keyboard::Escape)
                    return 0;
                break;

            case sf::Event::MouseButtonPressed:
                sf::Vector2f pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if (exitButton.contains(pos))
                    return 0;
                else if (localButton.contains(pos))
                    return 1;
                else if (onlineButton.contains(pos))
                    return 2;

                break;
            }
        }

        window.clear();

        window.draw(bg);
        window.draw(localButton);
        window.draw(onlineButton);
        window.draw(exitButton);

        window.display();

        sf::sleep(sf::milliseconds(20 - clock.getElapsedTime().asMilliseconds()));
        clock.restart();
    }

    return 0;
}
