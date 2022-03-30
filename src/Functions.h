#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include <string>
#include <cmath>

constexpr float dimRatio = 16.0f / 9.0f;
const std::string server_ip = "127.0.0.1";

void setText(sf::Text& txt, sf::Vector2f pos, sf::Color color, const sf::Font& fnt, const std::string& str);

sf::Vector2i getWindowSize();
