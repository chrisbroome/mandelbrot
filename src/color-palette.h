#ifndef COLOR_PALETTE_H
#define COLOR_PALETTE_H

#include <vector>
#include <SFML/graphics.hpp>

namespace mbv {

  using std::vector;
  using sf::Color;

  class ColorPalette {
    public:
      ColorPalette(unsigned int numColors);
      const Color& at(unsigned int index);
    private:
      const vector<Color> colors;
  };

};

#endif
