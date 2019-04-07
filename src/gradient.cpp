#include <cmath>
#include <iostream>
#include <vector>

#include "SFML/Graphics.hpp"

#include "gradient.h"

namespace mbv {
namespace gradient {
using sf::Color;
using std::log;
using std::ostream;
using std::sqrt;
using std::vector;

template <typename T> const Color operator+(const Color& color, const sf::Vector3<T> vector) {
  return Color(color.r + vector.x, color.g + vector.y, color.b + vector.z);
}
template <typename T> const Color operator+(const sf::Vector3<T> vector, const Color& color) {
  return Color(color.r + vector.x, color.g + vector.y, color.b + vector.z);
}

ostream& operator<<(ostream& out, const Color& color) {
  return out << "rgb"
             << "(" << (unsigned)color.r << "," << (unsigned)color.g << "," << (unsigned)color.b << ")";
}

template <typename T> ostream& operator<<(ostream& out, const sf::Vector3<T>& vect) {
  return out << "<" << vect.x << "," << vect.y << "," << vect.z << ">";
}

template <typename T> const sf::Vector3<T> toVect(const Color& c) { return sf::Vector3<T>(c.r, c.g, c.b); }

template <typename T> const Color toColor(const sf::Vector3<T>& v) { return Color(v.x, v.y, v.z); }

vector<Color> Linear(const Color& c1, const Color& c2, const unsigned int paletteSize) {
  const sf::Vector3<double> unit(1, 1, 1);
  const auto v1 = toVect<double>(c1);
  const auto v2 = toVect<double>(c2);
  const auto inc = (v2 - v1 + unit) / (double)paletteSize;
  sf::Vector3<double> ci(0, 0, 0);

  vector<Color> palette(paletteSize);
  for (auto& color : palette) {
    color = c1 + ci;
    ci += inc;
  }
  return palette;
}

vector<Color> Linear(const vector<Color>& colors, const unsigned int numColors) {
  if (numColors == 0) return vector<Color>(0);
  const auto n = colors.size();
  if (n == 0) return vector<Color>(0);
  const auto numGrads = n - 1;

  if (numColors == 1) return vector<Color>{colors[n / 2]};

  const auto paletteSegmentSize = numColors / numGrads;

  vector<Color> palette;
  for (unsigned int i = 0; i < numGrads; ++i) {
    const auto pal = Linear(colors.at(i), colors.at(i + 1), paletteSegmentSize);
    palette.insert(palette.end(), pal.begin(), pal.end());
  }
  return palette;
}

Color Smooth(const vector<Color>& palette, long double re, long double im, unsigned int i) {
  static const auto ONE_OVER_LOG2 = 1 / log(2);
  static const auto gradientScale = 256;
  static const auto gradientShift = 0;
  const auto size = sqrt(re * re + im * im);
  const auto smoothed = log(log(size) * ONE_OVER_LOG2) * ONE_OVER_LOG2;
  const auto colorI = (int)(sqrt(i + 1 - smoothed) * gradientScale + gradientShift) % palette.size();
  return palette.at(colorI);
}
}; // namespace gradient
}; // namespace mbv
