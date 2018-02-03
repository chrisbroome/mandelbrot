#ifndef GRADIENT_H
#define GRADIENT_H

#include <vector>
#include <SFML/Graphics.hpp>

namespace mbv {
  namespace gradient {
    using sf::Color;
    using std::ostream;
    using std::vector;
    vector<Color> Linear(const Color& c1, const Color& c2, const unsigned int paletteSize);
    vector<Color> Linear(const vector<Color>& colors, const unsigned int numColors);
  };
};

#endif
