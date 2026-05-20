#include <cmath>
#include <memory>
#include <optional>
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
  return out << name << "(" << r.position.x << ", " << r.position.y << ") (" << r.size.x << "," << r.size.y << ") " << std::endl;
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
  const auto grad = std::vector<sf::Color> {
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

class Gui {
  public:
    Gui(unsigned int width, unsigned int height) {
      texture = std::make_unique<sf::Texture>(sf::Vector2u{width, height});
      sprite = std::make_unique<sf::Sprite>(*texture);
      pixels = std::make_unique<sf::Image>();
      pixels->resize(texture->getSize());
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


static sf::Rect<world_coords_t> const initialView({-2, -1.25}, {2.5, 2.5});

class App {
public:
  App(unsigned int width, unsigned int height, unsigned int maxIterations)
  : window(sf::VideoMode({width, height}), "Mandelbrot Set Viewer")
  , palette(loadPalette(maxIterations))
  , gui(width, height)
  , screen({0, 0}, {(int)width, (int)height})
  , view(initialView)
  , dragStart(0, 0)
  , dragEnd(0, 0)
  , dragRectangle(sf::Vector2f(0, 0))
  , dragging(false)
  , viewShouldUpdate(false)
  {
    palette.at(palette.size() - 1) = sf::Color::Black;
    // blended blue color
    dragRectangle.setFillColor(sf::Color(0x80, 0x80, 0xff, 0x80));
  }

  ~App() {
    window.close();
  }

  void update() {
    viewShouldUpdate = true;
  }

  void resizeWindow(unsigned int width, unsigned int height) {
    // update the view to the new size of the window
    sf::FloatRect visibleArea({0.f, 0.f}, {(float)width, (float)height});
    window.setView(sf::View(visibleArea));

    gui.resize(width, height);
    screen.size.x = width;
    screen.size.y = height;
    update();
  }

  void resetView() {
    view = initialView;
    update();
  }

  std::optional<sf::Event> pollEvent() {
    return window.pollEvent();
  }

  void moveUp() {
    const auto step = 4;
    view.position.y -= view.size.y / step;
    update();
  }

  void moveDown() {
    const auto step = 4;
    view.position.y += view.size.y / step;
    update();
  }

  void moveLeft() {
    const auto step = 4;
    view.position.x -= view.size.x / step;
    update();
  }

  void moveRight() {
    const auto step = 4;
    view.position.x += view.size.x / step;
    update();
  }

  void zoomIn()  {
    // TODO: work out transforms for this
    // const auto center = getCenter(view);
    // const auto t = (view.top - center.y) / 2  - (view.height / 2);
    // const auto l = view.left + (view.width  / 2);
    // const auto h = view.height / 2;
    // const auto w = view.width / 2;
    // view = sf::FloatRect(l, t, w, h);
  }

  void startDrag(const sf::Vector2i& newDragStart) {
    dragging = true;
    dragStart = dragEnd = newDragStart;
  }

  void endDrag(const sf::Vector2i& newDragEnd) {
    dragging = false;
    dragEnd = newDragEnd;
    auto newTopLeft = translatePointFromTo(screen, view, dragStart);
    auto newBottomRight = translatePointFromTo(screen, view, dragEnd);
    pointPrintln(std::cout, newTopLeft, "newTopLeft");
    pointPrintln(std::cout, newBottomRight, "newBottomRight");

    const auto x = std::min(newTopLeft.x, newBottomRight.x);
    const auto y = std::min(newTopLeft.y, newBottomRight.y);

    const sf::Vector2<world_coords_t> newDimensions(
      std::fabs(newBottomRight.x - newTopLeft.x),
      std::fabs(newBottomRight.y - newTopLeft.y));
    const sf::Rect<world_coords_t> newView({x, y}, {newDimensions.x, newDimensions.y});
    const sf::Vector2<world_coords_t> scaleFactor(view.size.x / newView.size.x, view.size.y / newView.size.y);
    view = newView;
    rectPrintln(std::cout, view, "view");
    update();
  }

  void updateDrag(const sf::Vector2i& newDragEnd) {
    dragEnd = newDragEnd;
  }

  void render() {
    window.clear(sf::Color::White);
    if (viewShouldUpdate) {
      viewShouldUpdate = false;
      gui.updateView(view, palette);
    }
    gui.draw(window);
    if (dragging) {
      dragRectangle.setPosition(sf::Vector2f(dragStart));
      dragRectangle.setSize(sf::Vector2f(dragEnd - dragStart));
      window.draw(dragRectangle);
    } else {
      dragRectangle.setSize(sf::Vector2f(0, 0));
    }
    window.display();
  }

private:
  sf::RenderWindow window;
  std::vector<sf::Color> palette;
  Gui gui;
  sf::IntRect screen;
  sf::Rect<world_coords_t> view;
  sf::Vector2i dragStart;
  sf::Vector2i dragEnd;
  sf::RectangleShape dragRectangle;
  bool dragging;
  bool viewShouldUpdate;
};

class ApplicationEvent {
public:
  struct ResizeWindowEvent {
    unsigned int width;  ///< New width, in pixels
    unsigned int height; ///< New height, in pixels
  };
  struct DragStartEvent {
    int x;
    int y;
  };
  struct DragUpdateEvent {
    int x;
    int y;
  };
  struct DragEndEvent {
    int x;
    int y;
  };

  enum EventType {
    Null,
    ResizeWindow,
    ResetView,
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    ZoomIn,
    Quit,
    DragStart,
    DragUpdate,
    DragEnd,
  };

  EventType type;

  union {
    ResizeWindowEvent size;
    DragStartEvent    dragStart;
    DragUpdateEvent   dragUpdate;
    DragEndEvent      dragEnd;
  };

  static ApplicationEvent NewResizeWindow(unsigned int width, unsigned int height) {
    ApplicationEvent evt;
    evt.type = ApplicationEvent::EventType::ResizeWindow;
    evt.size = {width, height};
    return evt;
  }

  static ApplicationEvent NewDragStart(int x, int y) {
    ApplicationEvent evt;
    evt.type = ApplicationEvent::EventType::DragStart;
    evt.dragStart = {x, y};
    return evt;
  }

  static ApplicationEvent NewDragEnd(int x, int y) {
    ApplicationEvent evt;
    evt.type = ApplicationEvent::EventType::DragEnd;
    evt.dragEnd = {x, y};
    return evt;
  }

  static ApplicationEvent NewDragUpdate(int x, int y) {
    ApplicationEvent evt;
    evt.type = ApplicationEvent::EventType::DragUpdate;
    evt.dragUpdate = {x, y};
    return evt;
  }

  static ApplicationEvent NewReset() {
    return ApplicationEvent{ApplicationEvent::EventType::ResetView,{}};
  }
  static ApplicationEvent NewMoveUp() {
    return ApplicationEvent{ApplicationEvent::EventType::MoveUp,{}};
  }
  static ApplicationEvent NewMoveDown() {
    return ApplicationEvent{ApplicationEvent::EventType::MoveDown,{}};
  }
  static ApplicationEvent NewMoveLeft() {
    return ApplicationEvent{ApplicationEvent::EventType::MoveLeft,{}};
  }
  static ApplicationEvent NewMoveRight() {
    return ApplicationEvent{ApplicationEvent::EventType::MoveRight,{}};
  }
  static ApplicationEvent NewZoomIn() {
    return ApplicationEvent{ApplicationEvent::EventType::ZoomIn,{}};
  }
  static ApplicationEvent NewQuit() {
    return ApplicationEvent{ApplicationEvent::EventType::Quit,{}};
  }
  static ApplicationEvent NewNull() {
    return ApplicationEvent{ApplicationEvent::EventType::Null, {}};
  }
};

ApplicationEvent processInputEvents(const sf::Event& event) {
  if (const auto* e = event.getIf<sf::Event::Resized>())
    return ApplicationEvent::NewResizeWindow(e->size.x, e->size.y);
  if (const auto* e = event.getIf<sf::Event::KeyPressed>()) {
    if (e->code == sf::Keyboard::Key::R) return ApplicationEvent::NewReset();
    if (e->code == sf::Keyboard::Key::W) return ApplicationEvent::NewMoveUp();
    if (e->code == sf::Keyboard::Key::S) return ApplicationEvent::NewMoveDown();
    if (e->code == sf::Keyboard::Key::A) return ApplicationEvent::NewMoveLeft();
    if (e->code == sf::Keyboard::Key::D) return ApplicationEvent::NewMoveRight();
    if (e->code == sf::Keyboard::Key::Z) return ApplicationEvent::NewZoomIn();
    if (e->code == sf::Keyboard::Key::Escape) return ApplicationEvent::NewQuit();
  }
  if (event.is<sf::Event::Closed>()) return ApplicationEvent::NewQuit();
  if (const auto* e = event.getIf<sf::Event::MouseMoved>())
    return ApplicationEvent::NewDragUpdate(e->position.x, e->position.y);
  if (const auto* e = event.getIf<sf::Event::MouseButtonPressed>()) {
    if (e->button == sf::Mouse::Button::Left)
      return ApplicationEvent::NewDragStart(e->position.x, e->position.y);
  }
  if (const auto* e = event.getIf<sf::Event::MouseButtonReleased>()) {
    if (e->button == sf::Mouse::Button::Left)
      return ApplicationEvent::NewDragEnd(e->position.x, e->position.y);
  }
  return ApplicationEvent::NewNull();
}

int main() {
  const auto width = 1280;
  const auto height = 960;
  App app(width, height, 1024);
  app.update();

  // while window is open
  auto done = false;
  while (!done) {
    while (const auto event = app.pollEvent()) {
      auto e = processInputEvents(*event);
      if (e.type == ApplicationEvent::Null) {}
      else if (e.type == ApplicationEvent::ResizeWindow) app.resizeWindow(e.size.width, e.size.height);
      else if (e.type == ApplicationEvent::ResetView) app.resetView();
      else if (e.type == ApplicationEvent::MoveUp) app.moveUp();
      else if (e.type == ApplicationEvent::MoveDown) app.moveDown();
      else if (e.type == ApplicationEvent::MoveLeft) app.moveLeft();
      else if (e.type == ApplicationEvent::MoveRight) app.moveRight();
      else if (e.type == ApplicationEvent::DragStart) app.startDrag(sf::Vector2i(e.dragStart.x, e.dragStart.y));
      else if (e.type == ApplicationEvent::DragUpdate) app.updateDrag(sf::Vector2i(e.dragUpdate.x, e.dragUpdate.y));
      else if (e.type == ApplicationEvent::DragEnd) app.endDrag(sf::Vector2i(e.dragEnd.x, e.dragEnd.y));
      else if (e.type == ApplicationEvent::Quit) done = true;
    }
    app.render();
  }

  return 0;
}
