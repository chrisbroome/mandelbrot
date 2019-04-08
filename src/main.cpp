#include <cmath>
#include <memory>
#include <iostream>

#include "SFML/Graphics.hpp"

#include "gradient.h"
#include "transforms.h"

typedef long double world_coords_t;

template <typename T>
std::ostream& pointPrintln(std::ostream& out, const sf::Vector2<T>& p, const std::string& name = "") {
  return out << name << "(" << p.x << ", " << p.y << ")" << std::endl;
}

template <typename T> std::ostream& rectPrintln(std::ostream& out, const sf::Rect<T>& r, const std::string& name = "") {
  return out << name << "(" << r.left << ", " << r.top << ") (" << r.width << "," << r.height << ") " << std::endl;
}

int main() {
  const auto width = 1280;
  const auto height = 960;
  sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot Set Viewer");
  sf::Texture texture;
  if (!texture.create(width, height)) return -1;
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
  const auto grad = std::vector<sf::Color>{
      sf::Color::Red,     sf::Color(0xff, 0x7f, 0x00),
      sf::Color::Yellow,  sf::Color(0x7f, 0xff, 0x00),
      sf::Color::Green,   sf::Color::Blue,
      sf::Color::Magenta, sf::Color(0x7f, 0x00, 0x7f),
      sf::Color::Red,     sf::Color(0xff, 0x7f, 0x00),
      sf::Color::Yellow,  sf::Color(0x7f, 0xff, 0x00),
      sf::Color::Green,   sf::Color::Blue,
      sf::Color::Magenta, sf::Color(0x7f, 0x00, 0x7f),
      sf::Color::Red,     sf::Color(0xff, 0x7f, 0x00),
      sf::Color::Yellow,  sf::Color(0x7f, 0xff, 0x00),
      sf::Color::Green,   sf::Color::Blue,
      sf::Color::Magenta, sf::Color(0x7f, 0x00, 0x7f),
      sf::Color::Red,     sf::Color(0xff, 0x7f, 0x00),
      sf::Color::Yellow,  sf::Color(0x7f, 0xff, 0x00),
      sf::Color::Green,   sf::Color::Blue,
      sf::Color::Magenta, sf::Color(0x7f, 0x00, 0x7f),
  };

  const auto maxIterations = 1024;
  auto palette = mbv::gradient::Linear(grad, maxIterations);
  // palette.at(0) = sf::Color::Black;
  palette.at(palette.size() - 1) = sf::Color::Black;

  const sf::Vector2u textureSize = texture.getSize();
  const sf::IntRect screen(0, 0, textureSize.x, textureSize.y);
  auto pixels = std::make_unique<sf::Uint8[]>(textureSize.x * textureSize.y * 4);

  const sf::Rect<world_coords_t> initialView(-2, -1.25, 2.5, 2.5);
  auto view = initialView;

  updateViewTexture(pixels.get(), texture, view, palette);

  sf::Vector2<world_coords_t> newTopLeft(-2, -2);
  sf::Vector2<world_coords_t> newBottomRight(2, 2);
  sf::Vector2i mm(0, 0);
  sf::Vector2f dragStart(0, 0);
  sf::Vector2f dragEnd(0, 0);
  sf::RectangleShape dragRectangle(sf::Vector2f(0, 0));
  // blended blue color
  dragRectangle.setFillColor(sf::Color(0x80, 0x80, 0xff, 0x80));
  auto mousePressed = false;
  // while window is open
  auto done = false;
  while (!done) {

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Resized) {
        std::cout << "New dimensions: (" << event.size.width << "," << event.size.height << ")" << std::endl;
        std::cout << "TODO: implement window resizing" << std::endl;
      }
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
          // TODO: work out transforms for this
          // const auto center = getCenter(view);
          // const auto t = (view.top - center.y) / 2  - (view.height / 2);
          // const auto l = view.left + (view.width  / 2);
          // const auto h = view.height / 2;
          // const auto w = view.width / 2;
          // view = sf::FloatRect(l, t, w, h);
        }
        if (event.key.code == sf::Keyboard::Escape) {
          done = true;
        }
        updateViewTexture(pixels.get(), texture, view, palette);
      }
      if (event.type == sf::Event::Closed) {
        done = true;
      }
      if (event.type == sf::Event::MouseMoved) {
        mm = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
        dragEnd = sf::Vector2f(mm.x, mm.y);
      }
      if (event.type == sf::Event::MouseButtonPressed) {
        mousePressed = true;
        if (event.mouseButton.button == sf::Mouse::Left) {
          dragStart = dragEnd = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
          newTopLeft = translatePointFromTo(screen, view, mm);
          pointPrintln(std::cout, newTopLeft, "newTopLeft");
        }
      }
      if (event.type == sf::Event::MouseButtonReleased) {
        mousePressed = false;
        if (event.mouseButton.button == sf::Mouse::Left) {
          newBottomRight = translatePointFromTo(screen, view, mm);
          dragEnd = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
          pointPrintln(std::cout, newBottomRight, "newBottomRight");

          const auto x = std::min(newTopLeft.x, newBottomRight.x);
          const auto y = std::min(newTopLeft.y, newBottomRight.y);

          const sf::Vector2<world_coords_t> newDimensions(std::fabs(newBottomRight.x - newTopLeft.x),
                                                          std::fabs(newBottomRight.y - newTopLeft.y));
          const sf::Rect<world_coords_t> newView(x, y, newDimensions.x, newDimensions.y);
          const sf::Vector2<world_coords_t> scaleFactor(view.width / newView.width, view.height / newView.height);
          view = newView;
          rectPrintln(std::cout, view, "view");
          updateViewTexture(pixels.get(), texture, view, palette);
        }
      }
    }
    window.clear(sf::Color::White);
    window.draw(sprite);
    if (mousePressed) {
      dragRectangle.setPosition(dragStart);
      dragRectangle.setSize(dragEnd - dragStart);
      window.draw(dragRectangle);
    } else {
      dragRectangle.setSize(sf::Vector2f(0, 0));
    }
    window.display();
  }

  window.close();
  return 0;
}
