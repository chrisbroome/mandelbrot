#ifndef COLOR_PALETTE_H
#define COLOR_PALETTE_H

#include <SFML/Graphics.hpp>
#include <vector>

namespace mbv {

using sf::Color;
using std::vector;

class ColorPalette {
public:
  ColorPalette(unsigned int numColors);
  const Color& at(unsigned int index);

private:
  const vector<Color> colors;
};

}; // namespace mbv

#endif
