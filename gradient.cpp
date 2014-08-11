#include <vector>
#include <iostream>
#include <SFML/graphics.hpp>

#include "gradient.h"

template <typename T>
const sf::Color operator+(const sf::Color& color, const sf::Vector3<T> vector) {
  return sf::Color(color.r + vector.x, color.g + vector.y, color.b + vector.z);
}
template <typename T>
const sf::Color operator+(const sf::Vector3<T> vector, const sf::Color& color) {
  return sf::Color(color.r + vector.x, color.g + vector.y, color.b + vector.z);
}

std::ostream& operator <<(std::ostream& out, const sf::Color& color) {
  return out << "rgb"
    << "(" << (unsigned)color.r
    << "," << (unsigned)color.g
    << "," << (unsigned)color.b
    << ")";
}

template <typename T>
std::ostream& operator <<(std::ostream& out, const sf::Vector3<T>& vect) {
  return out
    << "<" << vect.x
    << "," << vect.y
    << "," << vect.z
    << ">";
}

template <typename T>
const sf::Vector3<T> toVect(const sf::Color& c) {
  return sf::Vector3<T>(c.r, c.g, c.b);
}

template <typename T>
const sf::Color toColor(const sf::Vector3<T>& v) {
  return sf::Color(v.x, v.y, v.z);
}


std::vector<sf::Color> Gradient::Linear(const sf::Color& c1, const sf::Color& c2, const unsigned int paletteSize) {
  const sf::Vector3<double> unit(1, 1, 1);
  const auto v1 = toVect<double>(c1);
  const auto v2 = toVect<double>(c2);
  const auto inc = (v2 - v1 + unit) / (double)paletteSize;
  sf::Vector3<double> ci(0, 0, 0);

  std::vector<sf::Color> palette(paletteSize);
  for (auto& color : palette) {
    color = c1 + ci;
    ci += inc;
  }
  return palette;
}

std::vector<sf::Color> Gradient::Linear(const std::vector<sf::Color>& colors, const unsigned int numColors) {
  if (numColors == 0) return std::vector<sf::Color>(0);
  const auto n = colors.size(); if (n == 0) return std::vector<sf::Color>(0);
  const auto numGrads = n - 1;

  if (numColors == 1) return (std::vector<sf::Color>){colors[n/2]};

  const auto paletteSegmentSize = numColors / numGrads;

  std::vector<sf::Color> palette;
  for (unsigned int i = 0; i < numGrads; ++i) {
    const auto pal = Gradient::Linear(colors.at(i), colors.at(i+1), paletteSegmentSize);
    palette.insert(palette.end(), pal.begin(), pal.end());
  }
  return palette;
}
