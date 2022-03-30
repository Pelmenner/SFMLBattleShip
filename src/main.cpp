#include "Connection.h"
#include "Functions.h"
#include "Resources.h"
#include "Button.h"
#include "Online.h"
#include "Local.h"

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <iostream>
#include <string>
#include <vector>

#pragma comment (linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

using std::vector;
using sf::Vector2i;
using sf::RenderWindow;
using std::string;
using std::pair;

bool enter_name(RenderWindow& wnd, string text, string& result);
int menu(sf::RenderWindow& window);

int SelectConnectionType(sf::RenderWindow& wnd);
int WaitConnection(sf::RenderWindow& wnd, Connection* mult);
int WaitOpponent(sf::RenderWindow& wnd, Connection* mult, std::string& opponentName);

class ResourceWrapper
{
public:
    ResourceWrapper() 
    {
        resources.loadResources();
    }

    ~ResourceWrapper()
    {
        resources.freeResources();
    }
};

int main()
{
    ResourceWrapper resourceWrapper;

    sf::Vector2i wndSize = GetWindowSize();

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
            if (!enter_name(wnd, "Enter first player's name: ", name1))
                break;

            if (!enter_name(wnd, "Enter second player's name: ", name2))
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
            if (!enter_name(wnd, "Enter your multiplayer nickname: ", nickname))
                break;

            bool ret = true;
            while (ret)
            {
                int type = SelectConnectionType(wnd);
                if (type == 0)
                    return 0;
                if (type == 4)
                    break;

                Connection session;

                if (type == 1)
                {
                    int res = WaitConnection(wnd, &session);
                    if (res == 0)
                        return 0;
                    if (res == 1)
                        continue;

                    Online game(wnd, &session);
                    game.setLocalName(nickname);
                    game.initPlayers();
                    session.SendName(nickname);
                    std::string opponent = "";
                    res = WaitOpponent(wnd, &session, opponent);
                    if (res == 0)
                        return 0;
                    game.SetOpponentName(opponent);

                    game.setLocalMove(0);
                    ret = game.play();

                    session.Disconnect();
                }
                else if (type == 2)
                {
                    std::string ip;
                    if (!enter_name(wnd, "Enter server ip address: ", ip))
                        break;

                    if (!session.ActiveConnection(ip))
                        continue;

                    Online game(wnd, &session);
                    game.InitLocal(nickname);
                    session.SendName(nickname);
                    std::string opponent = "";
                    int res = WaitOpponent(wnd, &session, opponent);
                    if (res == 0)
                        return 0;
                    game.SetOpponentName(opponent);

                    game.setLocalMove(1);
                    ret = game.play();

                    session.Disconnect();
                }
            }
        }
    }

    if (wnd.isOpen())
        wnd.close();

    return 0;
}

int WaitConnection(sf::RenderWindow& wnd, Connection* mult)
{
    sf::RectangleShape bg;
    bg.setFillColor(blackColor);
    bg.setSize(sf::Vector2f(1280.0f, 720.0f));

    sf::Text txt;
    txt.setString("Waiting for connection...");
    txt.setFont(*resources.mainFont);
    txt.setFillColor(grayColor);
    txt.setCharacterSize(80);
    txt.setPosition((sf::Vector2f(1280.0f, 720.0f) - sf::Vector2f(txt.getLocalBounds().width, txt.getLocalBounds().height)) / 2.0f);

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
                    mult->Disconnect();
                    return 1;
                }
            }
        }

        if (mult->PassiveConnection())
            break;

        wnd.clear();
        wnd.draw(bg);
        wnd.draw(txt);
        wnd.display();

        sf::sleep(sf::milliseconds(40));
    }

    return 2;
}

int WaitOpponent(sf::RenderWindow& wnd, Connection* mult, std::string& opponentName)
{
    sf::RectangleShape bg;
    bg.setSize(sf::Vector2f(1280.0f, 720.0f));
    bg.setFillColor(blackColor);

    sf::Text txt;
    txt.setString("Waiting for other player...");
    txt.setFont(*resources.mainFont);
    txt.setFillColor(grayColor);
    txt.setCharacterSize(80);
    txt.setPosition((sf::Vector2f(1280.0f, 720.0f) - sf::Vector2f(txt.getLocalBounds().width, txt.getLocalBounds().height)) / 2.0f);

    while (wnd.isOpen() && !mult->ReceiveName(opponentName))
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

        wnd.clear();
        wnd.draw(bg);
        wnd.draw(txt);
        wnd.display();

        sf::sleep(sf::milliseconds(40));
    }

    return 1;
}

int SelectConnectionType(sf::RenderWindow& wnd)
{
    sf::RectangleShape bg;
    bg.setSize(sf::Vector2f(1280.0f, 720.0f));
    bg.setFillColor(blackColor);

    sf::RectangleShape button_host;
    button_host.setSize(sf::Vector2f(800.0f, 100.0f));
    button_host.setOutlineColor(blueColor);
    button_host.setOutlineThickness(5);
    button_host.setFillColor(blackColor);
    button_host.setPosition(sf::Vector2f((1280 - button_host.getSize().x) / 2.0f, 200.0f));

    sf::Text text_host;
    text_host.setFont(*resources.mainFont);
    text_host.setFillColor(grayColor);
    text_host.setString("Host game");
    text_host.setCharacterSize(60);
    text_host.setPosition(sf::Vector2f((1280 - text_host.getLocalBounds().width) / 2.0f, 210.0f));

    sf::RectangleShape button_join;
    button_join.setSize(sf::Vector2f(800.0f, 100.0f));
    button_join.setOutlineColor(blueColor);
    button_join.setOutlineThickness(5);
    button_join.setFillColor(blackColor);
    button_join.setPosition(sf::Vector2f((1280 - button_join.getSize().x) / 2.0f, 350.0f));

    sf::Text text_join;
    text_join.setFillColor(grayColor);
    text_join.setString("Join game");
    text_join.setFont(*resources.mainFont);
    text_join.setCharacterSize(60);
    text_join.setPosition(sf::Vector2f((1280 - text_join.getLocalBounds().width) / 2.0f, 360.0f));

    sf::RectangleShape button_server;
    button_server.setSize(sf::Vector2f(800.0f, 100.0f));
    button_server.setOutlineColor(blueColor);
    button_server.setOutlineThickness(5);
    button_server.setFillColor(blackColor);
    button_server.setPosition(sf::Vector2f((1280 - button_server.getSize().x) / 2.0f, 500.0f));

    sf::Text text_server;
    text_server.setFillColor(grayColor);
    text_server.setString("Connect to server");
    text_server.setFont(*resources.mainFont);
    text_server.setCharacterSize(60);
    text_server.setPosition(sf::Vector2f((1280 - text_server.getLocalBounds().width) / 2.0f, 510.0f));

    sf::Text title;
    title.setFont(*resources.titleFont);
    title.setString("Choose multiplayer connection type: ");
    title.setFillColor(sf::Color::White);
    title.setCharacterSize(36);
    title.setPosition(sf::Vector2f((1280 - title.getLocalBounds().width) / 2.0f, 50.0f));
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
                    if (button_host.getGlobalBounds().contains(pos))
                        return 1;
                    if (button_join.getGlobalBounds().contains(pos))
                        return 2;
                }
            }
        }

        wnd.clear();

        wnd.draw(bg);
        wnd.draw(title);

        wnd.draw(button_host);
        wnd.draw(button_join);
        wnd.draw(button_server);

        wnd.draw(text_join);
        wnd.draw(text_host);
        wnd.draw(text_server);

        wnd.display();

        sf::sleep(sf::milliseconds(40));
    }
    return -1;
}

bool enter_name(RenderWindow& wnd, string text, string& result)
{
    sf::RectangleShape space;
    space.setSize({ 500, 80 });
    space.setOutlineColor(blueColor);
    space.setOutlineThickness(5);
    space.setFillColor(grayColor);
    space.setPosition((1280 - space.getSize().x) / 2.0f,
        (720 - space.getSize().y) / 2.0f * 0.8f);

    sf::RectangleShape bg;
    bg.setFillColor(blackColor);
    bg.setSize(sf::Vector2f(1280.0f, 720.0f));

    sf::Text title;
    title.setFont(*resources.mainFont);
    title.setString(text);
    title.setFillColor(sf::Color::White);
    title.setCharacterSize(1280 / 20);
    title.setPosition((1280 - title.getLocalBounds().width) / 2.0f,
        720 / 5.0f);

    Button okButton({ 1280 / 2.0f, 720 / 2.0f * 1.4f }, { 300, 120 });
    okButton.setText("OK");

    sf::Text txt;
    txt.setFont(*resources.mainFont);
    txt.setFillColor(txtColor);

    float textPositionY = space.getGlobalBounds().top + (space.getSize().y - txt.getGlobalBounds().height) / 2.0f - 20.0f;
    txt.setPosition(space.getGlobalBounds().left + (space.getSize().x - txt.getLocalBounds().width) / 2.0f, textPositionY);

    const int maxLength = 30;

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
                        return 1;
                }
                break;

            case sf::Event::KeyPressed:
                if (evnt.key.code == sf::Keyboard::Escape)
                    return 0;
                break;

            case sf::Event::TextEntered:
                switch (evnt.text.unicode)
                {
                case 0xD: //Return
                    return 1;
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

                txt.setPosition(space.getGlobalBounds().left +
                    (space.getLocalBounds().width - txt.getLocalBounds().width) / 2.0f, textPositionY);
            }
        }

        wnd.clear();
        wnd.draw(bg);
        wnd.draw(space);
        wnd.draw(txt);
        wnd.draw(title);
        wnd.draw(okButton);
        wnd.display();

        sf::sleep(sf::milliseconds(20 - clock.getElapsedTime().asMilliseconds()));
        clock.restart();
    }

    return 1;
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
                break;

            case sf::Event::KeyPressed:
                if (evnt.key.code == sf::Keyboard::Escape)
                    return 0;
                else if (evnt.key.code == sf::Keyboard::L)
                    return 1;
                else if (evnt.key.code == sf::Keyboard::O)
                    return 2;
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
