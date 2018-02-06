#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
// #include <gmpxx.h>
#include <mpreal.h>

#include "gradient.h"
#include "transforms.h"

typedef mpfr::mpreal world_coords_t;

template <typename T>
struct World {
  using Rect = sf::Rect<T>;
  using Vector = sf::Vector2<T>;
  const Rect world;
  Vector mm;
};

template <typename T>
std::ostream& pointPrintln(std::ostream& out, const sf::Vector2<T> &p, const std::string& name = "") {
  return out << name << "(" << p.x << ", " << p.y << ")" << std::endl;
}

template <typename T>
std::ostream& rectPrintln(std::ostream& out, const sf::Rect<T> &r, const std::string& name = "") {
  return out << name << "(" << r.left << ", " << r.top << ") (" << r.width << "," << r.height << ") " << std::endl;
}


int main() {
  const auto width = 1280;
  const auto height = 960;
  sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot Set Viewer");
  sf::Texture texture; if (!texture.create(width, height)) return -1;
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
    sf::Color::Red,
    sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Yellow,
    sf::Color(0x7f, 0xff, 0x00),
    sf::Color::Green,
    sf::Color::Blue,
    sf::Color::Magenta,
    sf::Color(0x7f, 0x00, 0x7f),
    sf::Color::Red,
    sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Yellow,
    sf::Color(0x7f, 0xff, 0x00),
    sf::Color::Green,
    sf::Color::Blue,
    sf::Color::Magenta,
    sf::Color(0x7f, 0x00, 0x7f),
    sf::Color::Red,
    sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Yellow,
    sf::Color(0x7f, 0xff, 0x00),
    sf::Color::Green,
    sf::Color::Blue,
    sf::Color::Magenta,
    sf::Color(0x7f, 0x00, 0x7f),
  };

  const auto maxIterations = 1024;
  auto palette = mbv::gradient::Linear(grad, maxIterations);
  // palette.at(0) = sf::Color::Black;
  palette.at(palette.size() - 1) = sf::Color::Black;

  const sf::Vector2u textureSize = texture.getSize();
  const sf::IntRect screen(0, 0, textureSize.x, textureSize.y);
  sf::Uint8 pixels[textureSize.x * textureSize.y * 4];

  const sf::Rect<world_coords_t> initialView(-2, -1.25, 2.5, 2.5);
  auto view = initialView;

  updateViewTexture(pixels, texture, view, palette);

  sf::Vector2<world_coords_t> newTopLeft(-2, -2);
  sf::Vector2<world_coords_t> newBottomRight(2, 2);
  sf::Vector2i mm(0, 0);
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
        mm = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
      }
      if (event.type == sf::Event::MouseButtonPressed) {
        mousePressed = true;
        if (event.mouseButton.button == sf::Mouse::Left) {
          newTopLeft = translatePointFromTo(screen, view, mm);
          pointPrintln(std::cout, newTopLeft, "newTopLeft");
        }
      }
      if (event.type == sf::Event::MouseButtonReleased) {
        mousePressed = false;
        if (event.mouseButton.button == sf::Mouse::Left) {
          newBottomRight = translatePointFromTo(screen, view, mm);
          pointPrintln(std::cout, newBottomRight, "newBottomRight");

          const sf::Vector2<world_coords_t> newDimensions(abs(newBottomRight.x - newTopLeft.x), abs(newBottomRight.y - newTopLeft.y));
          const sf::Rect<world_coords_t> newView(newTopLeft.x, newTopLeft.y, newDimensions.x, newDimensions.y);
          const sf::Vector2<world_coords_t> scaleFactor(view.width / newView.width, view.height / newView.height);
          view = newView;
          rectPrintln(std::cout, view, "view");
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

