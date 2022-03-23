#include "Game.h"
#include "Connection.h"
#include "Functions.h"

using std::string;
using sf::Vector2i;
using std::vector;

bool Game::playLocal()
{
	const sf::Vector2f buttonSize = sf::Vector2f(122.0f, 71.0f);

	sf::Font buttonFont;
	buttonFont.loadFromFile("TitleFont.ttf");

	Button replayButton({ 800.0f, 630.0f }, buttonSize, buttonFont, "Replay");
	Button exitButton({ 1000.0f, 630.0f }, buttonSize, buttonFont, "Exit");

	sf::RectangleShape bg;
	bg.setFillColor(blackColor);
	bg.setSize(sf::Vector2f(1280.0f, 720.0f));

	string add = "'s turn";
	sf::Text turn;
	setText(turn, { 20.0f, 680.0f }, nameColor, titleFont, board1->getName() + add);

	sf::Text name1(board1->getName(), titleFont), name2(board2->getName(), titleFont);
	name1.setFillColor(nameColor), name2.setFillColor(nameColor);
	name1.setPosition(300, 10), name2.setPosition(900, 10);

	sf::Text arrow;
	arrow.setOrigin(17.5, 17.5f);
	setText(arrow, { 600.0f, 350.0f }, nameColor, titleFont, "=>");

	int shot = 0;
	bool finished = false;

	board2->resetPosition(700, 100);
	board1->clearCells(), board2->clearCells();

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
			{
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

					if ((*board1)[pos].getFillColor() != blackColor) break;

					if (board1->hit(pos) == 0)
					{
						(*board1)[pos].setFillColor(grayColor);
						shot++;
						arrow.rotate(180.0f);
						turn.setString(board1->getName() + add);
					}
					else
						break;
				}
				else
				{
					pos.x = (pos.x - 690) / 50;
					pos.y = (pos.y - 100) / 50;

					if ((*board2)[pos].getFillColor() != blackColor) break;

					if (board2->hit(pos) == 0)
					{
						(*board2)[pos].setFillColor(grayColor);
						shot++;
						arrow.rotate(180.0f);
						turn.setString(board2->getName() + add);
					}
					else break;
				}
			}
			}
		}

		if (!finished && board1->hasLost())
		{
			finished = true;
			turn.setString(board2->getName() + " won!");
			board2->showAlive();
		}
		if (!finished && board2->hasLost())
		{
			finished = true;
			turn.setString(board1->getName() + " won!");
			board1->showAlive();
		}

		window.clear();

		window.draw(bg);
		window.draw(name1);
		window.draw(name2);
		window.draw(turn);

		board1->draw(window);
		board2->draw(window);
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

std::pair<Board*, Board*> Game::GetBoards()
{
	return std::pair<Board *, Board *>(board1, board2);
}

Game::Game(sf::RenderWindow &wnd, sf::Font &titleFont, sf::Font &fnt) :
	window(wnd), board1(new Board(50, 100)), board2(new Board(50, 100)), titleFont(titleFont), fnt(fnt)
{
	board1->setFont(fnt), board2->setFont(fnt);

	bg.setFillColor(blackColor), hider.setFillColor(sf::Color::Transparent);
	bg.setSize({ 1280, 720 }), hider.setSize({ 1280, 670 });
	hider.setPosition(0, 50);
}

void Game::resetShipNum(std::vector<sf::Text> &shipNum, std::vector<int> &countShips)
{
	for (unsigned int i = 0; i < shipNum.size(); ++i)
		shipNum[i].setString(std::to_string(4 - i - countShips[i]) + " x ");
}

void Game::setShipNum(std::vector<sf::Text> &shipNum, sf::Font &fnt)
{
	for (unsigned int i = 0; i < shipNum.size(); ++i)
	{
		shipNum[i].setFont(fnt);
		shipNum[i].setFillColor(sf::Color::White);
		shipNum[i].setPosition(700.0f, 100.0f + (float) i * 100.0f);
		shipNum[i].setString(std::to_string(4 - i) + " x ");
	}
}

void Game::setBlocks(std::vector<sf::RectangleShape> &blocks)
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
			blocks[(i * (i + 1)) / 2 + j].setPosition(775.0f + (float) j * 50.0f, 100.0f + (float) i * 100.0f);
}

void Game::initPlayer(Board *board)
{
	const sf::Vector2f buttonSize = sf::Vector2f(200.0f, 71.0f);
	sf::Font buttonFont;
	buttonFont.loadFromFile("data/TitleFont.ttf");

	Button randomButton({ 700.0f, 500.0f }, buttonSize, buttonFont, "Random");
	Button clearButton({ 1000.0f, 500.0f }, buttonSize, buttonFont, "Clear");
	Button acceptButton({ 850.0f, 600.0f }, buttonSize, buttonFont, "Accept");

	sf::Text name(board->getName(), titleFont);
	name.setPosition(100, 10);
	name.setFillColor(nameColor);

	bool drawing = false, ready = false;
	Vector2i buf_pos(0, 0);

	vector <sf::Text> shipNum(4);
	setShipNum(shipNum, titleFont);
	vector <sf::RectangleShape> blocks(10);
	setBlocks(blocks);

	sf::Clock clock;

	while (window.isOpen() && !ready)
	{
		sf::Event evnt;
		while (window.pollEvent(evnt))
		{
			switch (evnt.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (evnt.key.code == sf::Keyboard::Space)
				{
					if (hider.getFillColor() == sf::Color::Transparent)
						hider.setFillColor(blackColor);
					else
						hider.setFillColor(sf::Color::Transparent);
				}
			case sf::Event::MouseButtonPressed:
				if (hider.getFillColor() == blackColor)
					continue;
				sf::Vector2f pos_float = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				Vector2i pos(pos_float);

				if (randomButton.contains(pos_float) && evnt.mouseButton.button == sf::Mouse::Left)
				{
					board->RandomFill();
					break;
				}
				if (board->getShipNum() == 10 && acceptButton.contains(pos_float) && evnt.mouseButton.button == sf::Mouse::Left)
				{
					ready = true;
					break;
				}
				if (clearButton.contains(pos_float) && evnt.mouseButton.button == sf::Mouse::Left)
				{
					board->DeleteShips();
					break;
				}

				if (pos.y < 100 || pos.y > 600)
					break;
				if (pos.x > 550 || pos.x < 50)
					break;

				pos.x = (pos.x - 50) / 50;
				pos.y = (pos.y - 100) / 50;

				if (evnt.mouseButton.button == sf::Mouse::Left)
				{
					if (!drawing)
					{
						if (pos.x >= 0 && pos.x < 10 && pos.y >= 0 && pos.y < 10)
						{
							if ((*board)[pos].getFillColor() == blackColor)
							{
								(*board)[pos].setFillColor(yellowColor);
								drawing = true;
							}
						}

						buf_pos = pos;
					}
					else
					{

						if (!(pos.x >= 0 && pos.x < 10 && pos.y >= 0 && pos.y < 10 && board->addShip(buf_pos, pos)))
							(*board)[buf_pos].setFillColor(blackColor);
						drawing = false;
					}
				}
				else if (evnt.mouseButton.button == sf::Mouse::Right)
				{
					board->erase(pos);
				}
			}
		}

		resetShipNum(shipNum, board->getCountShips());

		window.clear();

		window.draw(bg);
		window.draw(name);

		if (board->getShipNum() == 10)
			window.draw(acceptButton);

		window.draw(randomButton);
		window.draw(clearButton);

		for (int i = 0; i < 10; ++i)
			window.draw(blocks[i]);
		for (int i = 0; i < 4; ++i)
			window.draw(shipNum[i]);

		board->draw(window);
		window.draw(hider);
		window.display();

		sf::sleep(sf::milliseconds(40 - clock.getElapsedTime().asMilliseconds()));
		clock.restart();
	}
}

void Game::setNames(const std::string &name1, const std::string &name2)
{
	board1->setName(name1), board2->setName(name2);
}

void Game::initPlayers()
{
	initPlayer(board1);
	initPlayer(board2);
}