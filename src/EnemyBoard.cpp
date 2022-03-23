#include "EnemyBoard.h"

void EnemyBoard::GetNeighbours(sf::Vector2i pos, std::vector<sf::Vector2i> &ans)
{
	if (pos.x > 0)
		ans.push_back(sf::Vector2i(pos.x - 1, pos.y));
	if (pos.y > 0)
		ans.push_back(sf::Vector2i(pos.x, pos.y - 1));
	if (pos.y < 9)
		ans.push_back(sf::Vector2i(pos.x, pos.y + 1));
	if (pos.x < 9)
		ans.push_back(sf::Vector2i(pos.x + 1, pos.y));
	if (pos.x > 0 && pos.y > 0)
		ans.push_back(sf::Vector2i(pos.x - 1, pos.y - 1));
	if (pos.x > 0 && pos.y < 9)
		ans.push_back(sf::Vector2i(pos.x - 1, pos.y + 1));
	if (pos.x < 9 && pos.y < 9)
		ans.push_back(sf::Vector2i(pos.x + 1, pos.y + 1));
	if (pos.x < 9 && pos.y > 0)
		ans.push_back(sf::Vector2i(pos.x + 1, pos.y - 1));
}

void EnemyBoard::SurroundKilled(sf::Vector2i pos, sf::Vector2i prev, int &len)
{
	if (cells[pos.x][pos.y].getFillColor() == redColor)
	{
		std::vector <sf::Vector2i> near;
		GetNeighbours(pos, near);
		++len;
		for (const sf::Vector2i &x : near)
			if (x != prev)
				SurroundKilled(x, pos, len);
	}
	else
	{
		cells[pos.x][pos.y].setFillColor(grayColor);
	}
}

EnemyBoard::EnemyBoard(int x, int y)
{
	ships_remaining.resize(4);
	for (int i = 0; i < 4; ++i)
		ships_remaining[i] = 4 - i;

	cells.resize(10);

	for (int i = 0; i < 10; ++i)
	{
		cells[i].resize(10);
		for (int j = 0; j < 10; ++j)
		{
			cells[i][j].setFillColor(blackColor);
			cells[i][j].setOutlineThickness(5);
			cells[i][j].setOutlineColor(blueColor);
			cells[i][j].setSize({ 50, 50 });
			cells[i][j].setPosition((float) x + (float) i * 50.0f, (float) y + (float) j * 50.0f);
		}
	}

	for (int i = 0; i < 10; ++i)
	{
		letters[i] = sf::Text(sf::String(char('A' + i)), font);
		numbers[i] = (i == 9) ? sf::Text(std::to_string(i + 1), font) : sf::Text(" " + std::to_string(i + 1), font);

		letters[i].setPosition(x + 50.0f * i + 25.0f, y - 50.0f);
		numbers[i].setPosition(x - 40.0f, y + 50.0f * i + 5.0f);

		numbers[i].setFillColor(sf::Color::White);
		letters[i].setFillColor(sf::Color::White);
	}
}

void EnemyBoard::Draw(sf::RenderWindow &wnd) const
{
	for (int i = 0; i < 10; ++i)
	{
		wnd.draw(letters[i]);
		wnd.draw(numbers[i]);
	}

	for (int i = 0; i < 10; ++i)
		for (int j = 0; j < 10; ++j)
			wnd.draw(cells[i][j]);
}

void EnemyBoard::SetFont(sf::Font fnt)
{
	font = fnt;
	for (int i = 0; i < 10; ++i)
	{
		letters[i].setFont(font);
		numbers[i].setFont(font);
	}
}

bool EnemyBoard::HasLost() const
{
	for (int i = 0; i < 4; ++i)
		if (ships_remaining[i] != 0)
			return false;
	return true;
}

void EnemyBoard::SetHit(sf::Vector2i pos, int hit)
{
	switch (hit)
	{
	case 0:
		cells[pos.x][pos.y].setFillColor(grayColor);
		break;
	case 1:
		cells[pos.x][pos.y].setFillColor(redColor);
		break;
	case 2:
		int length = 0;
		cells[pos.x][pos.y].setFillColor(redColor);
		SurroundKilled(pos, sf::Vector2i(-1, -1), length);
		ships_remaining[length - 1]--;
		break;
	}
}

void EnemyBoard::SetName(const std::string &name)
{
	this->name = name;
}

void EnemyBoard::ResetPosition(int x, int y)
{
	for (int i = 0; i < 10; ++i)
		for (int j = 0; j < 10; ++j)
			cells[i][j].setPosition((float) x + (float) i * 50.0f, (float) y + (float) j * 50.0f);

	for (int i = 0; i < 10; ++i)
	{
		letters[i].setPosition(x + 50.0f * i, y - 50.0f);
		numbers[i].setPosition(x - 50.0f, y + 50.0f * i);
	}
}

std::string EnemyBoard::GetName() const
{
	return name;
}

sf::RectangleShape &EnemyBoard::operator[](const sf::Vector2i &pos)
{
	return cells[pos.x][pos.y];
}

std::vector<sf::RectangleShape> &EnemyBoard::operator[](int x)
{
	return cells[x];
}
