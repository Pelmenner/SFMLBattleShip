#include "Field.h"

Field::Field(sf::Vector2f pos, const sf::Font& fnt) : position(pos), font(fnt)
{
    std::iota(std::rbegin(shipsCount), std::rend(shipsCount), 1);
    shipsCount[0] = 0;
    cells.resize(10);
    for (size_t i = 0; i < 10; ++i)
    {
        cells[i].resize(10);
        for (size_t j = 0; j < 10; ++j)
        {
            cells[i][j].setFillColor(blackColor);
            cells[i][j].setOutlineThickness(5.f);
            cells[i][j].setOutlineColor(blueColor);
            cells[i][j].setSize({ 50.f, 50.f });
        }
    }

    for (int i = 0; i < 10; ++i)
    {
        letters[i] = sf::Text(sf::String(char('A' + i)), font);
        numbers[i] = sf::Text(i == 9 ? "10" : ' ' + std::to_string(i + 1), font);

        letters[i].setFillColor(whiteColor);
        numbers[i].setFillColor(whiteColor);
    }

    name.setFont(font);

    updateLayout();
}

void Field::setName(const std::string& newName)
{
    name.setString(newName);
    sf::FloatRect bounds = name.getLocalBounds();
    name.setOrigin({ bounds.width / 2, 0.f });
    name.setPosition(position + sf::Vector2f(10 * 50.f / 2, -40.f));
}

void Field::setPosition(sf::Vector2f pos)
{
    position = pos;
    updateLayout();
}

void Field::clearCells()
{
    for (auto& line : cells)
        for (auto& cell : line)
            cell.setFillColor(blackColor);
}

bool Field::hasLost() const
{
    for (int shipSize = 1; shipSize <= 4; ++shipSize)
        if (shipsCount[shipSize] > 0)
            return false;
    return true;
}

const std::string& Field::getName() const
{
    return name.getString().toAnsiString();
}

sf::RectangleShape& Field::operator[](sf::Vector2i pos)
{
    return cells[pos.x][pos.y];
}

const sf::RectangleShape& Field::operator[](sf::Vector2i pos) const
{
    return cells[pos.x][pos.y];
}

std::vector<sf::RectangleShape>& Field::operator[](int i)
{
    return cells[i];
}

void Field::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (const auto& line : cells)
        for (const auto& cell : line)
            target.draw(cell);
    for (const auto& number : numbers)
        target.draw(number);
    for (const auto& letter : letters)
        target.draw(letter);
    target.draw(name);
}

int Field::surroundDestroyed(sf::Vector2i pos)
{
    int len = 0;
    dfs(pos, { -1, -1 }, len);
    return len;
}

void Field::getNeighbours(sf::Vector2i pos, std::vector<sf::Vector2i>& res)
{
    std::vector<sf::Vector2i> steps{
        { -1, -1 }, { -1, 0 }, { -1, 1 },
        { 0, -1 }, { 0, 1 },
        { 1, -1 }, { 1, 0 }, { 1, 1 }
    };
    for (const sf::Vector2i& step : steps)
    {
        sf::Vector2i buf = pos + step;
        if (buf.x >= 0 && buf.x < 10 &&
            buf.y >= 0 && buf.y < 10)
            res.push_back(buf);
    }
}

void Field::dfs(sf::Vector2i pos, sf::Vector2i prev, int& len)
{
    if (operator[](pos).getFillColor() == redColor)
    {
        ++len;
        std::vector<sf::Vector2i> adjacent;
        getNeighbours(pos, adjacent);
        for (const sf::Vector2i& nxt : adjacent)
            dfs(nxt, pos, len);
    }
    else
    {
        operator[](pos).setFillColor(grayColor);
    }
}

void Field::updateLayout()
{
    for (size_t i = 0; i < 10; ++i)
        for (size_t j = 0; j < 10; ++j)
            cells[i][j].setPosition(position.x + i * 50.f, position.y + j * 50.f);

    for (int i = 0; i < 10; ++i)
    {
        letters[i].setPosition(position.x + 50.f * i + 25.f, position.y - 50.f);
        numbers[i].setPosition(position.x - 40.f, position.y + 50.f * i + 5.f);
    }

    name.setPosition(position + sf::Vector2f(10 * 50.f / 2, -40.f));
}
