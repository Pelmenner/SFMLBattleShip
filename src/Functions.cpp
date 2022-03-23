#include "Functions.h"

void initTexture(sf::Texture*& texture, const std::string& path, bool smooth)
{
	texture = new sf::Texture();
	texture->loadFromFile(path);
	texture->setSmooth(smooth);
}

void setText(sf::Text& txt, sf::Vector2f pos, sf::Color color, sf::Font& fnt, const std::string& str)
{
	txt.setPosition(pos);
	txt.setFillColor(color);
	txt.setFont(fnt);
	if (!str.empty())
		txt.setString(str);
}

sf::Vector2i GetWindowSize()
{
	int width = GetSystemMetrics(SM_CXSCREEN) - 150;
	int height = GetSystemMetrics(SM_CYSCREEN) - 100;

	if (width >= 1280 && height >= 720)
		return sf::Vector2i(1280, 720);

	float ratio = float(width) / float(height);
	if (ratio > dimRatio)
		return sf::Vector2i((int) round(float(height) * dimRatio), height);
	else
		return sf::Vector2i(width, (int) round(float(width) / dimRatio));
}