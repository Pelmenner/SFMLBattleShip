#include "Online.h"
#include "Functions.h"
#include "Button.h"

Online::Online(sf::RenderWindow &wnd, sf::Font &titleFnt, sf::Font &fnt, Connection *mult) :
	wnd(wnd), local(new Board(50, 100)), opponent(new EnemyBoard(50, 100)), titleFnt(titleFnt), fnt(fnt), mult(mult)
{
	local->setFont(fnt);
	opponent->SetFont(fnt);

	bg.setFillColor(blackColor);
	bg.setSize(sf::Vector2f(wnd.getSize()));
}

void Online::SetShipNum(std::vector<sf::Text> &shipNum, sf::Font &fnt)
{
	for (unsigned int i = 0; i < shipNum.size(); ++i)
	{
		shipNum[i].setFont(fnt);
		shipNum[i].setFillColor(sf::Color::White);
		shipNum[i].setPosition(700.0f, 100.0f + (float) i * 100.0f);
		shipNum[i].setString(std::to_string(4 - i) + " x ");
	}
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

void Online::ResetShipNum(std::vector<sf::Text> &shipNum, std::vector<int> &countShips)
{
	for (unsigned int i = 0; i < shipNum.size(); ++i)
		shipNum[i].setString(std::to_string(4 - i - countShips[i]) + " x ");
}

void Online::InitLocal(const std::string &local_name)
{
	local->setName(local_name);
	
	const sf::Vector2f buttonSize = sf::Vector2f(200.0f, 71.0f);
	const sf::Font& buttonFont = titleFnt;

	Button randomButton({ 700.0f, 500.0f }, buttonSize, buttonFont, "Random");
	Button clearButton({ 1000.0f, 500.0f }, buttonSize, buttonFont, "Clear");
	Button acceptButton({ 850.0f, 600.0f }, buttonSize, buttonFont, "Accept");

	sf::Text name(local_name, titleFnt);
	name.setPosition(100, 10);
	name.setFillColor(nameColor);

	bool drawing = false, ready = false;
	sf::Vector2i buf_pos(0, 0);

	std::vector <sf::Text> shipNum(4);
	SetShipNum(shipNum, titleFnt);
	std::vector <sf::RectangleShape> cells(10);
	SetCells(cells);

	sf::Clock clock;

	while (wnd.isOpen() && !ready)
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
				sf::Vector2f pos_float = wnd.mapPixelToCoords(sf::Mouse::getPosition(wnd));
				sf::Vector2i pos(pos_float);

				if (randomButton.contains(pos_float) && evnt.mouseButton.button == sf::Mouse::Left)
				{
					local->RandomFill();
					break;
				}
				if (local->getShipNum() == 10 && acceptButton.contains(pos_float) && evnt.mouseButton.button == sf::Mouse::Left)
				{
					ready = true;
					break;
				}
				if (clearButton.contains(pos_float) && evnt.mouseButton.button == sf::Mouse::Left)
				{
					local->DeleteShips();
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
							if ((*local)[pos].getFillColor() == blackColor)
							{
								(*local)[pos].setFillColor(yellowColor);
								drawing = true;
							}
						}

						buf_pos = pos;
					}
					else
					{

						if (!(pos.x >= 0 && pos.x < 10 && pos.y >= 0 && pos.y < 10 && local->addShip(buf_pos, pos)))
							(*local)[buf_pos].setFillColor(blackColor);
						drawing = false;
					}
				}
				else if (evnt.mouseButton.button == sf::Mouse::Right)
				{
					local->erase(pos);
				}
			}
		}

		ResetShipNum(shipNum, local->getCountShips());

		wnd.clear();

		wnd.draw(bg);
		wnd.draw(name);

		if (local->getShipNum() == 10)
			wnd.draw(acceptButton);

		wnd.draw(randomButton);
		wnd.draw(clearButton);

		for (int i = 0; i < 10; ++i)
			wnd.draw(cells[i]);
		for (int i = 0; i < 4; ++i)
			wnd.draw(shipNum[i]);

		local->draw(wnd);
		wnd.display();

		sf::sleep(sf::milliseconds(40 - clock.getElapsedTime().asMilliseconds()));
		clock.restart();
	}
}

void Online::SetOpponentName(const std::string &name)
{
	opponent->SetName(name);
}

bool Online::Play(int localMove)
{
	const sf::Vector2f buttonSize = sf::Vector2f(122.0f, 71.0f);
	const sf::Font& buttonFont = titleFnt;

	Button replayButton({ 800.0f, 630.0f }, buttonSize, buttonFont, "Replay");
	Button exitButton({ 1000.0f, 630.0f }, buttonSize, buttonFont, "Exit");

	sf::RectangleShape bg;
	bg.setFillColor(blackColor);
	bg.setSize(sf::Vector2f(1280.0f, 720.0f));

	const std::string add = "'s turn";
	sf::Text turn;
	if (localMove == 0)
		setText(turn, { 20.0f, 680.0f }, nameColor, titleFnt, local->getName() + add);
	else
		setText(turn, { 20.0f, 680.0f }, nameColor, titleFnt, opponent->GetName() + add);

	sf::Text name1, name2;
	if (localMove == 0)
	{
		setText(name1, sf::Vector2f(300.0f, 10.0f), nameColor, titleFnt, local->getName());
		setText(name2, sf::Vector2f(900.0f, 10.0f), nameColor, titleFnt, opponent->GetName());
	}
	else
	{
		setText(name1, sf::Vector2f(300.0f, 10.0f), nameColor, titleFnt, opponent->GetName());
		setText(name2, sf::Vector2f(900.0f, 10.0f), nameColor, titleFnt, local->getName());
	}

	sf::Text arrow;
	arrow.setOrigin(17.5f, 17.5f);
	setText(arrow, { 600.0f, 350.0f }, nameColor, titleFnt, "=>");

	int shot = 0;
	bool finished = false;

	int state[10][10] = {};
	for (int i = 0; i < 10; ++i)
		for (int j = 0; j < 10; ++j)
			state[i][j] = (*local)[i][j].getFillColor() == redColor;

	if (localMove == 0)
		opponent->ResetPosition(700, 100);
	else
		local->resetPosition(700, 100);
	local->clearCells();

	sf::Clock clock;
	bool sent = false;
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
				sf::Vector2f pos_float = wnd.mapPixelToCoords(sf::Mouse::getPosition(wnd));
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

					opponent->SetHit(pos, hit);
					if (hit == 0)
					{
						++shot;
						arrow.rotate(180.0f);
						turn.setString(opponent->GetName() + add);
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
			turn.setString(opponent->GetName() + " won!");
			bool res = false;
			while (!res)
				res = mult->ReceiveBoard(*opponent);
			sent = true;
		}
		if (opponent->HasLost() && !sent)
		{
			finished = true;
			turn.setString(local->getName() + " won!");
			bool res = false;
			while (!res)
				res = mult->SendBoard(state);
			sent = true;
			local->showAlive();
		}

		wnd.clear();

		wnd.draw(bg);
		wnd.draw(name1);
		wnd.draw(name2);
		wnd.draw(turn);

		local->draw(wnd);
		opponent->Draw(wnd);
		wnd.draw(arrow);

		if (!finished)
			local->DrawShipBorders(wnd);
		else
		{
			wnd.draw(exitButton);
			wnd.draw(replayButton);
		}

		wnd.display();

		sf::sleep(sf::milliseconds(20 - clock.getElapsedTime().asMilliseconds()));
		clock.restart();
	}

	return false;
}
