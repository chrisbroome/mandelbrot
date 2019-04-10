#pragma once
#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include <iostream>

#include "SFML/Graphics.hpp"

template<typename T>
unsigned int mandelbrot(const sf::Vector2<T> c, const unsigned int numIterations);

template<typename T>
const T mandelbrotEscape(const sf::Vector2<T> z);

template<typename T>
const sf::Vector2<T> mandelbrotIteration(const sf::Vector2<T> z, const sf::Vector2<T> c);

template<typename T>
inline bool isInMainCardoid(const sf::Vector2<T>& p);

template<typename T, typename U>
sf::Transform getTransformFromTo(const sf::Rect<T> from, const sf::Rect<U> to);

template<typename T, typename U>
const sf::Vector2<U> translatePointFromTo(const sf::Rect<T> from, const sf::Rect<U> to, const sf::Vector2<T> point);

template<typename T> const sf::Vector2<T> getCenter(const sf::Rect<T> r);

template<typename T, typename U> sf::Transform getTransformFromTo(const sf::Rect<T> from, const sf::Rect<U> to) {
  const sf::Vector2f viewPoint(1.5, 1.5);
  const sf::Vector2f fromCenter(getCenter(from));
  const sf::Vector2f scaleFactor(to.width / from.width, to.height / from.height);
  const sf::Vector2f toCenter(getCenter(to));
  sf::Transform transform;
  transform.translate(-fromCenter).scale(scaleFactor).translate(toCenter);
  const auto translatedPoint = transform.transformPoint(viewPoint);
  std::cout << "(" << fromCenter.x << "x" << fromCenter.y << ")" << std::endl;
  std::cout << "(" << scaleFactor.x << "x" << scaleFactor.y << ")" << std::endl;
  std::cout << "(" << toCenter.x << "x" << toCenter.y << ")" << std::endl;
  std::cout << "(" << translatedPoint.x << "x" << translatedPoint.y << ")" << std::endl;

  return transform;
}

template<typename T, typename U>
const sf::Vector2<U> translatePointFromTo(const sf::Rect<T> from, const sf::Rect<U> to, const sf::Vector2<T> point) {
  const sf::Vector2f fc(getCenter(from));
  const sf::Vector2f tc(getCenter(to));

  const sf::Vector2f scaleFactor(to.width / from.width, to.height / from.height);
  const sf::Vector2<U> translatedPoint((point.x - fc.x) * scaleFactor.x + tc.x,
    (point.y - fc.y) * scaleFactor.y + tc.y
  );
  return translatedPoint;
}

template<typename T>
void updateViewTexture(
  sf::Image& pixels, sf::Texture& texture, const sf::Rect<T> view,
  const std::vector<sf::Color>& palette
) {
  const auto textureSize = texture.getSize();
  std::cerr << "updateViewTexture:textureSize:" << "(" << textureSize.x << "," << textureSize.y << ")" << std::endl;
  sf::Vector2u ti(0, 0);
  sf::Vector2<T> c(view.left, view.top);
  const sf::Vector2<T> cInc(view.width / textureSize.x, view.height / textureSize.y);
  for (ti.y = 0, c.y = view.top; ti.y < textureSize.y; ++ti.y, c.y += cInc.y) {
    for (ti.x = 0, c.x = view.left; ti.x < textureSize.x; ++ti.x, c.x += cInc.x) {
      const auto count = mandelbrot(c, palette.size() - 1);
      pixels.setPixel(ti.x, ti.y, palette.at(count));
    }
  }
  texture.update(pixels);
}

template<typename T> unsigned int mandelbrot(const sf::Vector2<T> c, const unsigned int numIterations) {
  if (c.x < -2 || c.x > 2 || c.y < -2 || c.y > 2) return numIterations;
  if (isInMainCardoid(c)) return numIterations;
  auto z = sf::Vector2<T>(0, 0);
  unsigned int count = 0;
  for (; count < numIterations; ++count) {
    z = mandelbrotIteration(z, c);
    if (mandelbrotEscape(z) > 4) return count;
  }
  return count;
}

template<typename T> inline bool isInMainCardoid(const sf::Vector2<T>& p) {
  const T xmq = p.x - .25;
  const T ysq = p.y * p.y;
  const T q = xmq * xmq + ysq;
  return q * (q + xmq) < .25 * ysq;
}

template<typename T> inline const T mandelbrotEscape(const sf::Vector2<T> z) { return z.x * z.x + z.y * z.y; }

template<typename T> inline const sf::Vector2<T> mandelbrotIteration(const sf::Vector2<T> z, const sf::Vector2<T> c) {
  return sf::Vector2<T>(z.x * z.x - z.y * z.y + c.x, 2 * z.x * z.y + c.y);
}

template<typename T> inline const sf::Vector2<T> getCenter(const sf::Rect<T> r) {
  return sf::Vector2<T>(r.left + r.width / 2, r.top + r.height / 2);
}

#endif
