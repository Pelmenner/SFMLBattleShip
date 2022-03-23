#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "SFML/System.hpp"
#include "SFML/Window.hpp"
#include "SFML/Audio.hpp"

#include "Game.h"
#include "Connection.h"
#include "Board.h"
#include "Functions.h"
#include "Online.h"
#include "Button.h"

#include <iostream>
#include <vector>
#include <string>

#pragma comment (linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

using std::vector;
using sf::Vector2i;
using sf::RenderWindow;
using std::string;
using std::pair;

bool enter_name(RenderWindow& wnd, string text, sf::Font& fnt, sf::Font& titleFnt, string& result);
int menu(sf::RenderWindow& window);

int SelectConnectionType(sf::RenderWindow& wnd, sf::Font& fnt, sf::Font& titleFnt);
int WaitConnection(sf::RenderWindow& wnd, Connection* mult, sf::Font& fnt);
int WaitOpponent(sf::RenderWindow& wnd, Connection* mult, sf::Font& fnt, std::string& opponentName);

int main()
{
    sf::Font fnt, titleFnt;
    fnt.loadFromFile("data/font.ttf");
    titleFnt.loadFromFile("data/TitleFont.ttf");

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
            sf::RectangleShape bg, hider;
            bg.setFillColor(blackColor), hider.setFillColor(sf::Color::Transparent);
            bg.setSize({ 1280, 720 }), hider.setSize({ 1280, 670 });
            hider.setPosition(0, 50);

            string name1, name2;
            if (!enter_name(wnd, "Enter first player's name: ", fnt, titleFnt, name1))
                break;

            if (!enter_name(wnd, "Enter second player's name: ", fnt, titleFnt, name2))
                break;

            bool res = true;
            while (res)
            {
                Game game(wnd, titleFnt, fnt);
                game.setNames(name1, name2);

                game.initPlayers();

                res = game.playLocal();
            }
        }
        else if (choice == 2)
        {
            std::string nickname;
            if (!enter_name(wnd, "Enter your multiplayer nickname: ", fnt, titleFnt, nickname))
                break;

            bool ret = true;
            while (ret)
            {
                int type = SelectConnectionType(wnd, fnt, titleFnt);
                if (type == 0)
                    return 0;
                if (type == 4)
                    break;

                Connection session;

                if (type == 1)
                {
                    int res = WaitConnection(wnd, &session, fnt);
                    if (res == 0)
                        return 0;
                    if (res == 1)
                        continue;

                    Online game(wnd, titleFnt, fnt, &session);
                    game.InitLocal(nickname);
                    session.SendName(nickname);
                    std::string opponent = "";
                    res = WaitOpponent(wnd, &session, fnt, opponent);
                    if (res == 0)
                        return 0;
                    game.SetOpponentName(opponent);

                    ret = game.Play(0);

                    session.Disconnect();
                }
                else if (type == 2)
                {
                    std::string ip;
                    if (!enter_name(wnd, "Enter server ip address: ", fnt, titleFnt, ip))
                        break;

                    if (!session.ActiveConnection(ip))
                        continue;

                    Online game(wnd, titleFnt, fnt, &session);
                    game.InitLocal(nickname);
                    session.SendName(nickname);
                    std::string opponent = "";
                    int res = WaitOpponent(wnd, &session, fnt, opponent);
                    if (res == 0)
                        return 0;
                    game.SetOpponentName(opponent);

                    ret = game.Play(1);

                    session.Disconnect();
                }
                else
                {
                    if (!session.ActiveConnection(server_ip))
                        continue;

                    int local_turn = 0;
                    if (!session.ReceiveTurn(local_turn))
                        continue;

                    Online game(wnd, titleFnt, fnt, &session);
                    game.InitLocal(nickname);
                    session.SendName(nickname);
                    std::string opponent = "";
                    int res = WaitOpponent(wnd, &session, fnt, opponent);
                    if (res == 0)
                        return 0;
                    game.SetOpponentName(opponent);

                    ret = game.Play(local_turn);

                    session.Disconnect();
                }
            }
        }
    }

    if (wnd.isOpen())
        wnd.close();

    return 0;
}

int WaitConnection(sf::RenderWindow& wnd, Connection* mult, sf::Font& fnt)
{
    sf::RectangleShape bg;
    bg.setFillColor(blackColor);
    bg.setSize(sf::Vector2f(1280.0f, 720.0f));

    sf::Text txt;
    txt.setString("Waiting for connection...");
    txt.setFont(fnt);
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

int WaitOpponent(sf::RenderWindow& wnd, Connection* mult, sf::Font& fnt, std::string& opponentName)
{
    sf::RectangleShape bg;
    bg.setSize(sf::Vector2f(1280.0f, 720.0f));
    bg.setFillColor(blackColor);

    sf::Text txt;
    txt.setString("Waiting for other player...");
    txt.setFont(fnt);
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

int SelectConnectionType(sf::RenderWindow& wnd, sf::Font& fnt, sf::Font& titleFnt)
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
    text_host.setFont(fnt);
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
    text_join.setFont(fnt);
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
    text_server.setFont(fnt);
    text_server.setCharacterSize(60);
    text_server.setPosition(sf::Vector2f((1280 - text_server.getLocalBounds().width) / 2.0f, 510.0f));

    sf::Text title;
    title.setFont(titleFnt);
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
            case sf::Event::MouseButtonPressed:
                if (evnt.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2f pos = wnd.mapPixelToCoords(sf::Mouse::getPosition(wnd));
                    if (button_host.getGlobalBounds().contains(pos))
                        return 1;
                    if (button_join.getGlobalBounds().contains(pos))
                        return 2;
                    /*if (button_server.getGlobalBounds().contains(pos))
                        return 3;*/
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

bool enter_name(RenderWindow& wnd, string text, sf::Font& fnt, sf::Font& titleFnt, string& result)
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
    title.setFont(fnt);
    title.setString(text);
    title.setFillColor(sf::Color::White);
    title.setCharacterSize(1280 / 20);
    title.setPosition((1280 - title.getLocalBounds().width) / 2.0f,
        720 / 5.0f);

    Button okButton({ 1280 / 2.0f, 720 / 2.0f * 1.4f }, { 300, 120 }, titleFnt);
    okButton.setText("OK");

    sf::Text txt;
    txt.setFont(fnt);
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

    sf::Font buttonFont;
    buttonFont.loadFromFile("data/TitleFont.ttf");
    const sf::Vector2f buttonSize = sf::Vector2f(244, 142);
    Button localButton({ 1280 / 3.0f, 720 / 3.0f }, buttonSize, buttonFont);
    Button onlineButton({ 1280 / 3.0f * 2.0f, 720 / 3.0f }, buttonSize, buttonFont);
    Button exitButton({ 1280 / 2.0f, 720 / 3.0f * 2.0f }, buttonSize, buttonFont);

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