#ifndef GRADIENT_H
#define GRADIENT_H

#include <vector>
#include <SFML/graphics.hpp>

class Gradient {
public:
  static std::vector<sf::Color> Linear(const sf::Color& c1, const sf::Color& c2, const unsigned int paletteSize);
  static std::vector<sf::Color> Linear(const std::vector<sf::Color>& colors, const unsigned int numColors);
};

#endif
