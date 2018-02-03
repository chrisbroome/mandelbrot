#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#include "gradient.h"

template <typename T>
unsigned int mandelbrot(const sf::Vector2<T> c, const unsigned int numIterations);

template <typename T>
const T mandelbrotEscape(const sf::Vector2<T> z);

template <typename T>
const sf::Vector2<T> mandelbrotIteration(const sf::Vector2<T> z, const sf::Vector2<T> c);

template <typename T>
void updateViewTexture(sf::Uint8* pixels, sf::Texture& texture, const sf::Rect<T> view, const std::vector<sf::Color>& palette);

template <typename T, typename U>
sf::Transform getTransformFromTo(const sf::Rect<T> from, const sf::Rect<U> to);

template <typename T, typename U>
const sf::Vector2<U> translatePointFromTo(const sf::Rect<T> from, const sf::Rect<U> to, const sf::Vector2<T> point);

template <typename T>
const sf::Vector2<T> getCenter(const sf::Rect<T> r);

int main() {

  sf::RenderWindow window(sf::VideoMode(1024, 768), "Mandelbrot Set Viewer");
  sf::Texture texture; if (!texture.create(1024, 768)) return -1;
  auto sprite = sf::Sprite(texture);

  /*
    Red     ff   0   0
    Orange  ff  7f   0
    Yellow  ff  ff   0
    Y - G   7f  ff   0
    Green    0  ff   0
    Blue     0   0  ff
    Purple  ff   0  ff
    Pink    7f   0  7f
    Black    0   0   0
  */
  const auto grad = std::vector<sf::Color> {
    sf::Color::Red,
    sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Yellow,
    sf::Color(0x7f, 0xff, 0x00),
    sf::Color::Green,
    sf::Color::Blue,
    sf::Color::Magenta,
    sf::Color(0x7f, 0x00, 0x7f),
    sf::Color::Black
  };

  auto palette = mbv::gradient::Linear(grad, 256);
  palette.at(0) = sf::Color::Black;

  const sf::Vector2u textureSize = texture.getSize();
  const sf::FloatRect textureRect(0, 0, textureSize.x, textureSize.y);
  sf::Uint8 pixels[textureSize.x * textureSize.y * 4];

  const sf::FloatRect initialView(-2, -1.25, 2.5, 2.5);
  auto view = initialView;

  updateViewTexture(pixels, texture, view, palette);

  sf::Vector2f newTopLeft(-2, -2);
  sf::Vector2f newBottomRight(2, 2);
  sf::Vector2f mm(0, 0);
  auto mousePressed = false;
  // while window is open
  while(window.isOpen()) {

    sf::Event event;
    while(window.pollEvent(event)) {
      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Key::R) {
          view = initialView;
        }
        const auto step = 4;
        // move up
        if (event.key.code == sf::Keyboard::Key::W) {
          view.top -= view.height / step;
        }
        // move down
        if (event.key.code == sf::Keyboard::Key::S) {
          view.top += view.height / step;
        }
        // move left
        if (event.key.code == sf::Keyboard::Key::A) {
          view.left -= view.width / step;
        }
        // move right
        if (event.key.code == sf::Keyboard::Key::D) {
          view.left += view.width / step;
        }
        // zoom in
        if (event.key.code == sf::Keyboard::Key::Z) {
          //TODO: work out transforms for this
          // const auto center = getCenter(view);
          // const auto t = (view.top - center.y) / 2  - (view.height / 2);
          // const auto l = view.left + (view.width  / 2);
          // const auto h = view.height / 2;
          // const auto w = view.width / 2;
          // view = sf::FloatRect(l, t, w, h);
        }
        updateViewTexture(pixels, texture, view, palette);
      }
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (event.type == sf::Event::MouseMoved) {
        mm = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
      }
      if (event.type == sf::Event::MouseButtonPressed) {
        mousePressed = true;
        if (event.mouseButton.button == sf::Mouse::Left) {
          newTopLeft = translatePointFromTo(textureRect, view, mm);
          std::cout << "newTopLeft(" << newTopLeft.x << ", " << newTopLeft.y << ")" << std::endl;
        }
      }
      if (event.type == sf::Event::MouseButtonReleased) {
        mousePressed = false;
        if (event.mouseButton.button == sf::Mouse::Left) {
          newBottomRight = translatePointFromTo(textureRect, view, mm);
          std::cout << "newBottomRight(" << newBottomRight.x << ", " << newBottomRight.y << ")" << std::endl;
          const sf::Vector2f newDimensions(fabs(newBottomRight.x - newTopLeft.x), fabs(newBottomRight.y - newTopLeft.y));
          const sf::FloatRect newView(newTopLeft.x, newTopLeft.y, newDimensions.x, newDimensions.y);
          const sf::Vector2f scaleFactor(view.width / newView.width, view.height / newView.height);
          view = newView;
          std::cout << "view(" << view.left << ", " << view.top << ") (" << view.width << "," << view.height << ") " << std::endl;
          updateViewTexture(pixels, texture, view, palette);
        }
      }
    }
    window.clear(sf::Color::White);
    window.draw(sprite);
    window.display();
  }

  return 0;
}

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
  for (; count < numIterations; ++count)
  {
    if (mandelbrotEscape(z) > 4) return count;
    z = mandelbrotIteration(z, c);
  }
  return mandelbrotEscape(z) < 4 ? 0 : count;
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
