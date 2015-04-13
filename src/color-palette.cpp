#include <vector>
#include <SFML/graphics.hpp>

#include "color-palette.h"

namespace mbv {
  using std::vector;
  using sf::Color;

  ColorPalette::ColorPalette(unsigned int numColors) : colors(numColors) {};
  const Color& ColorPalette::at(unsigned int index) { return colors.at(index); };

};
