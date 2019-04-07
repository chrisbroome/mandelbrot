#include <SFML/Graphics.hpp>
#include <vector>

#include "color-palette.h"

namespace mbv {
using sf::Color;
using std::vector;

ColorPalette::ColorPalette(unsigned int numColors) : colors(numColors){};
const Color& ColorPalette::at(unsigned int index) { return colors.at(index); };

}; // namespace mbv
