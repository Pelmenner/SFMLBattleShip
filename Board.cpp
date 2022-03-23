#include "Board.h"

int Board::checkHit(sf::Vector2i position)
{
	for (unsigned int i = 0; i < ships.size(); i++)
	{
		if (ships[i].direction == 0)
		{
			if (position.y != ships[i].pos.y)
				continue;
			if (position.x < ships[i].pos.x || position.x >= ships[i].pos.x + ships[i].length)
				continue;

			return i;
		}
		else
		{
			if (position.x != ships[i].pos.x)
				continue;
			if (position.y < ships[i].pos.y || position.y >= ships[i].pos.y + ships[i].length)
				continue;

			return i;
		}
	}
	return -1;
}

void Board::surroundKilled(int index)
{
	sf::Vector2i start = ships[index].pos;
	sf::Vector2i end;
	end.x = start.x + ((ships[index].direction + 1) % 2) * ships[index].length;
	end.y = start.y + ships[index].direction * ships[index].length;

	sf::Vector2i start1 = start;
	std::vector <sf::Vector2i> currCells;
	while (start1 != end)
	{
		getNeighbours(start1, currCells);
		start1.x += (ships[index].direction + 1) % 2;
		start1.y += ships[index].direction % 2;
	}

	for (auto coordinates : currCells)
		cells[coordinates.x][coordinates.y].setFillColor(grayColor);

	while (start != end)
	{
		cells[start.x][start.y].setFillColor(redColor);
		start.x += (ships[index].direction + 1) % 2;
		start.y += ships[index].direction % 2;
	}
}

void Board::getNeighbours(sf::Vector2i pos, std::vector<sf::Vector2i> &ans)
{
	if (pos.x > 0 && pos.y > 0)
		ans.push_back({ pos.x - 1, pos.y - 1 });
	if (pos.x > 0)
		ans.push_back({ pos.x - 1, pos.y });
	if (pos.x > 0 && pos.y < 9)
		ans.push_back({ pos.x - 1, pos.y + 1 });
	if (pos.y > 0)
		ans.push_back({ pos.x, pos.y - 1 });
	if (pos.y > 0 && pos.x < 9)
		ans.push_back({ pos.x + 1, pos.y - 1 });
	if (pos.x < 9 && pos.y < 9)
		ans.push_back({ pos.x + 1, pos.y + 1 });
	if (pos.x < 9)
		ans.push_back({ pos.x + 1, pos.y });
	if (pos.y < 9)
		ans.push_back({ pos.x, pos.y + 1 });
}

Board::Board(int x, int y)
{
	count_ships.resize(4);
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

void Board::resetPosition(int x, int y)
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

void Board::draw(sf::RenderWindow &window) const
{
	for (int i = 0; i < 10; ++i)
		for (int j = 0; j < 10; ++j)
			window.draw(cells[i][j]);

	for (int i = 0; i < 10; ++i)
	{
		window.draw(letters[i]);
		window.draw(numbers[i]);
	}
}

void Board::updateCells()
{
	for (unsigned int i = 0; i < 10; ++i)
		for (unsigned int j = 0; j < 10; ++j)
			cells[i][j].setFillColor(blackColor);
	for (unsigned int i = 0; i < ships.size(); i++)
		surroundKilled(i);
}

void Board::showAlive()
{
	for (auto &ship : ships)
	{
		sf::Vector2i start = ship.pos;
		sf::Vector2i end = start;
		end.x += ((ship.direction + 1) % 2) * ship.length;
		end.y += ship.direction * ship.length;
		while (start != end)
		{
			if (cells[start.x][start.y].getFillColor() == blackColor)
				cells[start.x][start.y].setFillColor(greenColor);

			start.x += (ship.direction + 1) % 2;
			start.y += ship.direction;
		}
	}
}

void Board::DrawShipBorders(sf::RenderWindow &wnd)
{
	for (const auto &ship : ships)
	{
		if (ship.health == 0)
			continue;

		sf::Vector2i pos = ship.pos;
		for (int i = 0; i < ship.length; ++i)
		{
			sf::RectangleShape buf = this->operator[](pos);
			buf.setOutlineColor(redColor);
			wnd.draw(buf);

			if (ship.direction == 0)
				++pos.x;
			else
				++pos.y;
		}
	}
}

int Board::hit(sf::Vector2i &position)
{
	// -1 - wrong move 
	// 0 - empty cell
	// 1 - damaged a ship
	// 2 - destroyed a ship

	if (cells[position.x][position.y].getFillColor() != blackColor)
		return -1;

	int res = checkHit(position);
	if (res == -1)
		return 0;


	cells[position.x][position.y].setFillColor(redColor);
	--ships[res].health;

	if (ships[res].health == 0)
	{
		surroundKilled(res);
		return 2;
	}
	else
	{
		return 1;
	}
}

bool Board::hasLost()
{
	for (auto ship : ships)
		if (ship.health)
			return false;

	return true;
}

bool Board::addShip(sf::Vector2i begin, sf::Vector2i end)
{
	if (begin.x > end.x || begin.y > end.y)
		std::swap(begin, end);
	if (end.x - begin.x && end.y - begin.y)
		return false;

	int direction = (int) (end.y != begin.y);
	int length = (end.x - begin.x) + (end.y - begin.y) + 1;

	if (length > 4)
		return false;
	if (count_ships[length - 1] == 4 - length + 1)
		return false;

	sf::Vector2i begin1 = begin;
	while (begin != end)
	{
		if (cells[begin.x][begin.y].getFillColor() == grayColor)
			return false;
		begin.x += (direction + 1) % 2;
		begin.y += direction % 2;
	}
	if (cells[begin.x][begin.y].getFillColor() == grayColor)
		return false;

	ships.push_back(ship(begin1, end));

	begin = begin1;
	std::vector<sf::Vector2i> currCells;
	while (begin != end)
	{
		getNeighbours(begin, currCells);
		begin.x += (direction + 1) % 2;
		begin.y += direction % 2;
	}
	getNeighbours(begin, currCells);

	for (auto coordinates : currCells)
		cells[coordinates.x][coordinates.y].setFillColor(grayColor);

	begin = begin1;
	while (begin != end)
	{
		cells[begin.x][begin.y].setFillColor(redColor);
		begin.x += (direction + 1) % 2;
		begin.y += direction % 2;
	}
	cells[begin.x][begin.y].setFillColor(redColor);

	count_ships[length - 1]++;

	return true;
}

void Board::RandomFill()
{
	srand(time(0));
	DeleteShips();

	for (int len = 4; len > 0; --len)
	{
		int num = 5 - len;
		for (int i = 0; i < num;)
		{
			sf::Vector2i begin(rand() % 10, rand() % 10);
			if (operator[](begin).getFillColor() != blackColor)
				continue;

			int dir = rand() % 2;
			sf::Vector2i end(begin.x + (!dir) * (len - 1), begin.y + dir * (len - 1));
			if (end.x >= 10 || end.y >= 10)
				continue;
			if (operator[](end).getFillColor() != blackColor)
				continue;

			if (addShip(begin, end))
				++i;
		}
	}
}

void Board::DeleteShips()
{
	ships.clear();
	count_ships.assign(4, 0);
	clearCells();
}

bool Board::fromHash(const std::string &s)
{
	if (s.empty())
		return false;

	ships.clear();
	int n = s[0];

	if (s.length() != n * 5 + 102)
		return false;

	for (int i = 0; i < n; ++i)
	{
		ship newShip;
		newShip.pos.x = s[i * 5 + 1];
		newShip.pos.y = s[i * 5 + 2];
		newShip.direction = s[i * 5 + 3];
		newShip.length = s[i * 5 + 4];
		newShip.health = s[i * 5 + 5];

		ships.push_back(newShip);
	}

	if (s[n * 5 + 1] != '#')
		return false;

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			switch (s[n * 6 + i * 10 + j + 2])
			{
			case 1:
				cells[i][j].setFillColor(blackColor);
				break;
			case 2:
				cells[i][j].setFillColor(redColor);
				break;
			case 3:
				cells[i][j].setFillColor(grayColor);
				break;
			default:
				return false;
			}
		}
	}

	return true;
}

int Board::getShipNum()
{
	return count_ships[0] + count_ships[1] +
		count_ships[2] + count_ships[3];
}

std::string Board::getName()
{
	return name;
}

std::string Board::hash() const
{
	std::string s;
	s.push_back((char) ships.size());

	for (auto ship : ships)
	{
		s.push_back((char) ship.pos.x);
		s.push_back((char) ship.pos.y);
		s.push_back((char) ship.direction);
		s.push_back((char) ship.length);
		s.push_back((char) ship.health);
	}

	s.push_back('#');

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			if (cells[i][j].getFillColor() == blackColor)
				s.push_back(1);
			else if (cells[i][j].getFillColor() == redColor)
				s.push_back(2);
			else
				s.push_back(3);
		}
	}

	return s;
}

std::vector<int> &Board::getCountShips()
{
	return count_ships;
}

std::vector<sf::RectangleShape> &Board::operator[](int index)
{
	return cells[index];
}

sf::RectangleShape &Board::operator[](sf::Vector2i pos)
{
	return cells[pos.x][pos.y];
}

void Board::erase(sf::Vector2i pos)
{
	for (unsigned int i = 0; i < ships.size(); i++)
	{
		if (ships[i].direction == 0)
		{
			if (pos.y != ships[i].pos.y)
				continue;
			if (pos.x < ships[i].pos.x || pos.x > ships[i].pos.x + ships[i].length - 1)
				continue;

			count_ships[ships[i].length - 1]--;
			ships.erase(ships.begin() + i);
			break;
		}
		else
		{
			if (pos.x != ships[i].pos.x)
				continue;
			if (pos.y < ships[i].pos.y || pos.y > ships[i].pos.y + ships[i].length - 1)
				continue;

			count_ships[ships[i].length - 1]--;
			ships.erase(ships.begin() + i);
			break;
		}
	}
	updateCells();
}

void Board::clearCells()
{
	for (int i = 0; i < 10; ++i)
		for (int j = 0; j < 10; ++j)
			cells[i][j].setFillColor(blackColor);
}

void Board::setFont(sf::Font &newFont)
{
	font = newFont;
}

Board::ship::ship(sf::Vector2i start, sf::Vector2i end)
{
	if (start.x > end.x || start.y > end.y)
		std::swap(start, end);

	health = length = (end.x - start.x) + (end.y - start.y) + 1;
	direction = (int) (end.y != start.y);
	pos = start;
}