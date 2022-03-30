#include "Functions.h"
#include "Resources.h"
#include "Button.h"
#include "Online.h"

Online::Online(sf::RenderWindow& wnd, Connection* mult) :
	Game(wnd), 
	local(new LocalField({ 50, 100 })),
	opponent(new RemoteField({ 50, 100 })),
	mult(mult), localMove(0)
{
}

void Online::SetCells(std::vector<sf::RectangleShape> &cells)
{
	for (int i = 0; i < 10; ++i)
	{
		cells[i].setFillColor(blackColor);
		cells[i].setOutlineColor(sf::Color::White);
		cells[i].setOutlineThickness(5);
		cells[i].setSize({ 50, 50 });
	}

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j <= i; ++j)
			cells[(i * (i + 1)) / 2 + j].setPosition(775.0f + (float) j * 50.0f, 100.0f + (float) i * 100.0f);
}

void Online::InitLocal(const std::string &local_name)
{
	local->setName(local_name);
	initPlayer(local);
}

void Online::SetOpponentName(const std::string &name)
{
	opponent->setName(name);
}

void Online::initPlayers()
{
	InitLocal(local->getName());
}

void Online::setLocalMove(int move)
{
	localMove = move;
}

void Online::setLocalName(const std::string& name)
{
	local->setName(name);
}

std::pair<Field*, Field*> Online::getFields() const
{
	return { local, opponent };
}

bool Online::play()
{
	const sf::Vector2f buttonSize = sf::Vector2f(122.0f, 71.0f);

	Button replayButton({ 800.0f, 665.0f }, buttonSize, "Replay");
	Button exitButton({ 1000.0f, 665.0f }, buttonSize, "Exit");

	const std::string add = "'s turn";
	sf::Text turn;
	if (localMove == 0)
		setText(turn, { 20.0f, 680.0f }, nameColor, resources::titleFont, local->getName() + add);
	else
		setText(turn, { 20.0f, 680.0f }, nameColor, resources::titleFont, opponent->getName() + add);

	sf::Text name1, name2;
	if (localMove == 0)
	{
		setText(name1, sf::Vector2f(300.0f, 10.0f), nameColor, resources::titleFont, local->getName());
		setText(name2, sf::Vector2f(900.0f, 10.0f), nameColor, resources::titleFont, opponent->getName());
	}
	else
	{
		setText(name1, sf::Vector2f(300.0f, 10.0f), nameColor, resources::titleFont, opponent->getName());
		setText(name2, sf::Vector2f(900.0f, 10.0f), nameColor, resources::titleFont, local->getName());
	}

	sf::Text arrow;
	arrow.setOrigin(17.5f, 17.5f);
	setText(arrow, { 600.0f, 350.0f }, nameColor, resources::titleFont, "=>");

	int shot = 0;
	bool finished = false;

	int state[10][10] = {};
	for (int i = 0; i < 10; ++i)
		for (int j = 0; j < 10; ++j)
			state[i][j] = (*local)[i][j].getFillColor() == redColor;

	if (localMove == 0)
		opponent->setPosition({ 700, 100 });
	else
		local->setPosition({ 700, 100 });
	local->clearCells();

	sf::Clock clock;
	bool sent = false;
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
					if (exitButton.contains(pos_float) && evnt.mouseButton.button == sf::Mouse::Left)
						return false;
					if (replayButton.contains(pos_float) && evnt.mouseButton.button == sf::Mouse::Left)
						return true;
					break;
				}

				if (shot % 2 == localMove)
				{
					if (shot % 2 == 1 && (pos.x > 550 || pos.x < 50)) break;
					if (shot % 2 == 0 && (pos.x < 690 || pos.x > 1190)) break;
					if (pos.y < 100 || pos.y > 600) break;

					if (shot % 2 == 1)
						pos = (pos - sf::Vector2i(50, 100)) / 50;
					else
						pos = (pos - sf::Vector2i(690, 100)) / 50;

					if ((*opponent)[pos].getFillColor() != blackColor)
						break;

					bool is_sent = false;
					while (!is_sent)
						is_sent = mult->SendMove(pos.x, pos.y);

					bool is_received = false;
					int hit = -1;
					while (!is_received)
						is_received = mult->ReceiveResponse(hit);

					opponent->setHit(pos, hit);
					if (hit == 0)
					{
						++shot;
						arrow.rotate(180.0f);
						turn.setString(opponent->getName() + add);
					}
					else break;
				}
			}
		}

		if (shot % 2 != localMove)
		{
			sf::Vector2i pos = {};
			if (mult->ReceiveMove(pos.x, pos.y))
			{
				int hit = local->hit(pos);
				bool is_sent = false;
				while (!is_sent)
					is_sent = mult->SendResponse(hit);

				if (hit == 0)
				{
					(*local)[pos].setFillColor(grayColor);
					++shot;
					arrow.rotate(180.0f);
					turn.setString(local->getName() + add);
				}
			}
		}

		if (local->hasLost() && !sent)
		{
			finished = true;
			turn.setString(opponent->getName() + " won!");
			bool res = false;
			while (!res)
				res = mult->ReceiveField(*opponent);
			sent = true;
		}
		if (opponent->hasLost() && !sent)
		{
			finished = true;
			turn.setString(local->getName() + " won!");
			bool res = false;
			while (!res)
				res = mult->SendField(state);
			sent = true;
			local->showRemainingShips();
		}

		window.clear(blackColor);

		window.draw(name1);
		window.draw(name2);
		window.draw(turn);

		window.draw(*local);
		window.draw(*opponent);
		window.draw(arrow);

		if (!finished)
			local->showShipBorders();
		else
		{
			window.draw(exitButton);
			window.draw(replayButton);
		}

		window.display();

		sf::sleep(sf::milliseconds(20 - clock.getElapsedTime().asMilliseconds()));
		clock.restart();
	}

	return false;
}
