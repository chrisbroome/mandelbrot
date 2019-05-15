#include <cmath>
#include <exception>
#include <memory>
#include <iostream>

#include "SFML/Graphics.hpp"

#include "gradient.h"
#include "transforms.h"

typedef long double world_coords_t;

template<typename T>
std::ostream& pointPrintln(std::ostream& out, const sf::Vector2<T>& p, const std::string& name = "") {
  return out << name << "(" << p.x << ", " << p.y << ")" << std::endl;
}

template<typename T>
std::ostream& rectPrintln(std::ostream& out, const sf::Rect<T>& r, const std::string& name = "") {
  return out << name << "(" << r.left << ", " << r.top << ") (" << r.width << "," << r.height << ") " << std::endl;
}

std::vector<sf::Color> loadPalette(const unsigned int maxIterations) {
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
    sf::Color::Red, sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Yellow, sf::Color(0x7f, 0xff, 0x00),
    sf::Color::Green, sf::Color::Blue,
    sf::Color::Magenta, sf::Color(0x7f, 0x00, 0x7f),
    sf::Color::Red, sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Yellow, sf::Color(0x7f, 0xff, 0x00),
    sf::Color::Green, sf::Color::Blue,
    sf::Color::Magenta, sf::Color(0x7f, 0x00, 0x7f),
    sf::Color::Red, sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Yellow, sf::Color(0x7f, 0xff, 0x00),
    sf::Color::Green, sf::Color::Blue,
    sf::Color::Magenta, sf::Color(0x7f, 0x00, 0x7f),
    sf::Color::Red, sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Yellow, sf::Color(0x7f, 0xff, 0x00),
    sf::Color::Green, sf::Color::Blue,
    sf::Color::Magenta, sf::Color(0x7f, 0x00, 0x7f),
    sf::Color::Red, sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Yellow, sf::Color(0x7f, 0xff, 0x00),
    sf::Color::Green, sf::Color::Blue,
    sf::Color::Magenta, sf::Color(0x7f, 0x00, 0x7f),
    sf::Color::Red, sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Yellow, sf::Color(0x7f, 0xff, 0x00),
    sf::Color::Green, sf::Color::Blue,
    sf::Color::Magenta, sf::Color(0x7f, 0x00, 0x7f),
    sf::Color::Red, sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Yellow, sf::Color(0x7f, 0xff, 0x00),
    sf::Color::Green, sf::Color::Blue,
    sf::Color::Magenta, sf::Color(0x7f, 0x00, 0x7f),
    sf::Color::Red, sf::Color(0xff, 0x7f, 0x00),
    sf::Color::Yellow, sf::Color(0x7f, 0xff, 0x00),
    sf::Color::Green, sf::Color::Blue,
    sf::Color::Magenta, sf::Color(0x7f, 0x00, 0x7f),
  };

  return mbv::gradient::Linear(grad, maxIterations);
}

class TextureCreationFailure : public std::exception {
private:
  unsigned int width;
  unsigned int height;
public:
  TextureCreationFailure(unsigned int width, unsigned int height) : std::exception()
  , width(width)
  , height(height) {}

  const char* what() const noexcept override {
    return "Error creating texture";
  }

  unsigned int Width() const {
    return width;
  }

  unsigned int Height() const {
    return height;
  }
};

class Gui {
  public:
    Gui(unsigned int width, unsigned int height) {
      texture = std::make_unique<sf::Texture>();
      if (!texture->create(width, height)) throw TextureCreationFailure(width, height);
      sprite = std::make_unique<sf::Sprite>(*texture);
      const sf::Vector2u textureSize = texture->getSize();
      sf::IntRect screen(0, 0, textureSize.x, textureSize.y);
      pixels = std::make_unique<sf::Image>();
      pixels->create(texture->getSize().x, texture->getSize().y);
    }

    template<typename T>
    void updateView(const sf::Rect<T> view, const std::vector<sf::Color>& palette) {
      updateViewTexture(*pixels, *texture, view, palette);
    }

    void resize(unsigned int width, unsigned int height) {
      swap(Gui(width, height));
    }

    void swap(Gui&& other) {
      pixels.swap(other.pixels);
      texture.swap(other.texture);
      sprite.swap(other.sprite);
    }

    void draw(sf::RenderWindow& window) {
      window.draw(*sprite);
    }

  private:
    std::unique_ptr<sf::Texture> texture;
    std::unique_ptr<sf::Sprite> sprite;
    std::unique_ptr<sf::Image> pixels;
};

int main() {
  const auto width = 1280;
  const auto height = 960;
  sf::RenderWindow window(sf::VideoMode(width, height), "Mandelbrot Set Viewer");
  auto maxIterations = 1024 * 2;
  auto palette = loadPalette(maxIterations);
  // palette.at(0) = sf::Color::Black;
  palette.at(palette.size() - 1) = sf::Color::Black;

  Gui gui(width, height);
  sf::IntRect screen(0, 0, width, height);

  const sf::Rect<world_coords_t> initialView(-2, -1.25, 2.5, 2.5);
  auto view = initialView;

  gui.updateView(view, palette);

  sf::Vector2<world_coords_t> newTopLeft(-2, -2);
  sf::Vector2<world_coords_t> newBottomRight(2, 2);
  sf::Vector2i dragStart(0, 0);
  sf::Vector2i dragEnd(0, 0);
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
        const auto s = event.size;
        // update the view to the new size of the window
        sf::FloatRect visibleArea(0, 0, s.width, s.height);
        window.setView(sf::View(visibleArea));

        gui.resize(s.width, s.height);
        screen.width = s.width;
        screen.height = s.height;
        gui.updateView(view, palette);
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
        gui.updateView(view, palette);
      }
      if (event.type == sf::Event::Closed) {
        done = true;
      }
      if (event.type == sf::Event::MouseMoved) {
        dragEnd = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
      }
      if (event.type == sf::Event::MouseButtonPressed) {
        mousePressed = true;
        if (event.mouseButton.button == sf::Mouse::Left) {
          dragStart = dragEnd = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
          newTopLeft = translatePointFromTo(screen, view, dragStart);
          pointPrintln(std::cout, newTopLeft, "newTopLeft");
        }
      }
      if (event.type == sf::Event::MouseButtonReleased) {
        mousePressed = false;
        if (event.mouseButton.button == sf::Mouse::Left) {
          dragEnd = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
          newBottomRight = translatePointFromTo(screen, view, dragEnd);
          pointPrintln(std::cout, newBottomRight, "newBottomRight");

          const auto x = std::min(newTopLeft.x, newBottomRight.x);
          const auto y = std::min(newTopLeft.y, newBottomRight.y);

          const sf::Vector2<world_coords_t> newDimensions(
            std::fabs(newBottomRight.x - newTopLeft.x),
            std::fabs(newBottomRight.y - newTopLeft.y));
          const sf::Rect<world_coords_t> newView(x, y, newDimensions.x, newDimensions.y);
          const sf::Vector2<world_coords_t> scaleFactor(view.width / newView.width, view.height / newView.height);
          view = newView;
          rectPrintln(std::cout, view, "view");
          gui.updateView(view, palette);
        }
      }
    }
    window.clear(sf::Color::White);
    gui.draw(window);
    if (mousePressed) {
      dragRectangle.setPosition(sf::Vector2f(dragStart));
      dragRectangle.setSize(sf::Vector2f(dragEnd - dragStart));
      window.draw(dragRectangle);
    } else {
      dragRectangle.setSize(sf::Vector2f(0, 0));
    }
    window.display();
  }

  window.close();
  return 0;
}
