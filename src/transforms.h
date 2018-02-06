#ifndef TRANSFORMS_H
#define TRANSFORMS_H

#include <SFML/Graphics.hpp>
#include <iostream>

template <typename T>
unsigned int mandelbrot(const sf::Vector2<T> c, const unsigned int numIterations);

template <typename T>
const T mandelbrotEscape(const sf::Vector2<T> z);

template <typename T>
const sf::Vector2<T> mandelbrotIteration(const sf::Vector2<T> z, const sf::Vector2<T> c);

template <typename T>
inline bool isInMainCardoid(const sf::Vector2<T>& p);

template <typename T>
void updateViewTexture(sf::Uint8* pixels, sf::Texture& texture, const sf::Rect<T> view, const std::vector<sf::Color>& palette);

template <typename T, typename U>
sf::Transform getTransformFromTo(const sf::Rect<T> from, const sf::Rect<U> to);

template <typename T, typename U>
const sf::Vector2<U> translatePointFromTo(const sf::Rect<T> from, const sf::Rect<U> to, const sf::Vector2<T> point);

template <typename T>
const sf::Vector2<T> getCenter(const sf::Rect<T> r);



template <typename T, typename U>
sf::Transform getTransformFromTo(const sf::Rect<T> from, const sf::Rect<U> to) {
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

template <typename T, typename U>
const sf::Vector2<U> translatePointFromTo(const sf::Rect<T> from, const sf::Rect<U> to, const sf::Vector2<T> point) {
  const sf::Vector2f f(from.left, from.top);
  const sf::Vector2f t(to.left, to.top);

  const sf::Vector2f fc(getCenter(from));
  const sf::Vector2f tc(getCenter(to));

  const sf::Vector2f scaleFactor(to.width / from.width, to.height / from.height);
  const sf::Vector2<U> translatedPoint(
    (point.x - fc.x) * scaleFactor.x + tc.x,
    (point.y - fc.y) * scaleFactor.y + tc.y
  );
  std::cout << "  (x,y)" << std::endl;
  std::cout << " p(" << point.x << "," << point.y << ")" << std::endl;
  std::cout << " f(" << f.x << "," << f.y << ")" << " fc(" << fc.x << "," << fc.y << ")" << std::endl;
  std::cout << " t(" << t.x << "," << t.y << ")" << " tc(" << tc.x << "," << tc.y << ")" << std::endl;
  std::cout << "sf(" << scaleFactor.x << "," << scaleFactor.y << ")" << std::endl;
  std::cout << "tp(" << translatedPoint.x << "," << translatedPoint.y << ")" << std::endl;
  return translatedPoint;
}

template <typename T>
void updateViewTexture(sf::Uint8* pixels, sf::Texture& texture, const sf::Rect<T> view, const std::vector<sf::Color>& palette) {
  const auto textureSize = texture.getSize();
  sf::Vector2u ti;
  sf::Vector2<T> c(view.left, view.top);
  const sf::Vector2<T> cInc(view.width / textureSize.x, view.height / textureSize.y);
  unsigned int i = 0;
  for(ti.y = 0, c.y = view.top; ti.y < textureSize.y; ++ti.y, c.y += cInc.y) {
    for(ti.x = 0, c.x = view.left; ti.x < textureSize.x; ++ti.x, c.x += cInc.x, i += 4) {
      const auto count = mandelbrot(c, palette.size()-1);
      const sf::Color color = palette.at(count);
      pixels[i] = color.r;
      pixels[i+1] = color.g;
      pixels[i+2] = color.b;
      pixels[i+3] = color.a;
    }
  }
  texture.update(pixels);
}

template <typename T>
unsigned int mandelbrot(const sf::Vector2<T> c, const unsigned int numIterations)
{
  unsigned int count = 0;
  sf::Vector2<T> z(0, 0);

  if (c.x < -2 || c.x > 2 || c.y < -2 || c.y > 2) return 0;
  if (isInMainCardoid(c)) return 0;
  for (; count < numIterations; ++count)
  {
    if (mandelbrotEscape(z) > 4) return count;
    z = mandelbrotIteration(z, c);
  }
  return mandelbrotEscape(z) < 4 ? 0 : count;
}

template <typename T>
inline bool isInMainCardoid(const sf::Vector2<T>& p) {
  const T xmq = p.x - .25;
  const T ysq = p.y * p.y;
  const T q = xmq * xmq + ysq;
  return q * (q + xmq) < .25 * ysq;
}

template <typename T>
const T mandelbrotEscape(const sf::Vector2<T> z) {
  return z.x*z.x + z.y*z.y;
}

template <typename T>
const sf::Vector2<T> mandelbrotIteration(const sf::Vector2<T> z, const sf::Vector2<T> c) {
  return sf::Vector2<T>(
    z.x*z.x - z.y*z.y + c.x,
    2 * z.x*z.y + c.y
  );
}

template <typename T>
const sf::Vector2<T> getCenter(const sf::Rect<T> r) {
  return sf::Vector2<T>(
    r.left + r.width / 2,
    r.top + r.height / 2
  );
}

#endif
