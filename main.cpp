#include <SFML/Graphics.hpp>
#include <iostream>

template <typename T>
unsigned int mandelbrot(const sf::Vector2<T> c, const unsigned int numIterations);

template <typename T>
const T mandelbrotEscape(const sf::Vector2<T> z);

template <typename T>
const sf::Vector2<T> mandelbrotIteration(const sf::Vector2<T> z, const sf::Vector2<T> c);

template <typename T>
void updateViewTexture(sf::Uint8* pixels, sf::Texture& texture, const sf::Rect<T> view);

template <typename T, typename U>
sf::Transform getTransformFromTo(const sf::Rect<T> from, const sf::Rect<U> to);

template <typename T, typename U>
const sf::Vector2<U> translatePointFromTo(const sf::Rect<T> from, const sf::Rect<U> to, const sf::Vector2<T> point);

template <typename T>
const sf::Vector2<T> getCenter(const sf::Rect<T> r);

void mouseCoordinates(const sf::Event& event, const sf::Transform& transformFromTextureToView, const sf::FloatRect& view, const sf::Texture& texture);

int main() {

  sf::RenderWindow window(sf::VideoMode(640, 480), "Mandelbrot Set Viewer");
  sf::Texture texture; if (!texture.create(640, 480)) return -1;
  sf::Sprite sprite(texture);

  const sf::Vector2u textureSize = texture.getSize();
  const sf::IntRect textureRect(0, 0, textureSize.x, textureSize.y);
  sf::Uint8 pixels[textureSize.x * textureSize.y * 4];

  sf::FloatRect view(-2, -2, 4, 4);

  auto transformFromTextureToView = getTransformFromTo(textureRect, view);
  updateViewTexture(pixels, texture, view);

  // while window is open
  while(window.isOpen()) {

    sf::Event event;
    while(window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      if (event.type == sf::Event::MouseMoved) {
        const sf::Vector2f mm(event.mouseMove.x, event.mouseMove.y);
        const auto textureSize = texture.getSize();
        const auto textureRect = sf::FloatRect(0, 0, textureSize.x, textureSize.y);
        const auto newRect = transformFromTextureToView.transformRect(textureRect);
        const auto vc = transformFromTextureToView.transformPoint(sf::Vector2f(mm.x, mm.y));
        const sf::Vector2f newViewDimensions(view.width / 2, view.height / 2);
        const auto newPoint = translatePointFromTo(textureRect, view, mm);
//        mouseCoordinates(event, transformFromTextureToView, view, texture);

//        const sf::Vector2u mm(event.mouseMove.x, event.mouseMove.y);
//        const auto textureSize = texture.getSize();
//        const auto textureRect = sf::FloatRect(0, 0, textureSize.x, textureSize.y);
//        const auto newRect = transformFromTextureToView.transformRect(textureRect);
//        const auto vc = transformFromTextureToView.transformPoint(sf::Vector2f(mm.x, mm.y));
//        const sf::Vector2f newViewDimensions(view.width / 2, view.height / 2);
//        std::cout << "Mouse Center: (" << mm.x << "," << mm.y << ") " << "(" << textureRect.width << "x" << textureRect.height << ") ";
//        std::cout << "View Center: (" << vc.x << "," << vc.y << ") " << "(" << view.width << "x" << view.height << ") ";
//        std::cout << "New View Dimensions: (" << vc.x << "," << vc.y << ") " << "(" << newViewDimensions.x << "x" << newViewDimensions.y << ") ";
//        std::cout << "New Rect: (" << newRect.left << "," << newRect.top << ")" << "(" << newRect.width << "x" << newRect.height << ") ";
//        std::cout << std::endl;
      }
      if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
          mouseCoordinates(event, transformFromTextureToView, view, texture);
          const sf::Vector2u mm(event.mouseMove.x, event.mouseMove.y);
          const auto textureSize = texture.getSize();
          const auto textureRect = sf::FloatRect(0, 0, textureSize.x, textureSize.y);
          const auto newRect = transformFromTextureToView.transformRect(textureRect);
          const auto vc = transformFromTextureToView.transformPoint(sf::Vector2f(mm.x, mm.y));
          const sf::Vector2f newViewDimensions(view.width / 2, view.height / 2);
          std::cout << "Mouse Center: (" << mm.x << "," << mm.y << ") " << "(" << textureRect.width << "x" << textureRect.height << ") ";
          std::cout << "View Center: (" << vc.x << "," << vc.y << ") " << "(" << view.width << "x" << view.height << ") ";
          std::cout << "New View Dimensions: (" << vc.x << "," << vc.y << ") " << "(" << newViewDimensions.x << "x" << newViewDimensions.y << ") ";
          std::cout << "New Rect: (" << newRect.left << "," << newRect.top << ")" << "(" << newRect.width << "x" << newRect.height << ") ";
          std::cout << std::endl;
          view = sf::FloatRect(vc, newViewDimensions);
          transformFromTextureToView = getTransformFromTo(textureRect, view);
          updateViewTexture(pixels, texture, view);
        }
      }
    }
    window.clear(sf::Color::White);
    window.draw(sprite);
    window.display();
  }

  return 0;
}

// mm is mouse position in texture coordinates
// Vector2f r1c = r1.center
// Vector2f r2c = r2.center
// Vector2f sf(r1.width / r2.width, r1.height / r2.height)
// zoom.p1.x =  (sf.x * (mm.x + r1c.x - r2c.x)) + r1c.x;
// zoom.p1.y = -(sf.y * (mm.y + r1c.y - r2c.y)) + r1c.y;

template <typename T, typename U>
sf::Transform getTransformFromTo(const sf::Rect<T> from, const sf::Rect<U> to) {
  const sf::Vector2f viewPoint(1.5, 1.5);
  const sf::Vector2f fromCenter(from.left + from.width / 2, from.top + from.height / 2);
  const sf::Vector2f scaleFactor(to.width / from.width, -to.height / from.height);
  const sf::Vector2f toCenter(to.left + to.width / 2, to.top + to.height / 2);
  sf::Transform transform;
  transform.translate(-fromCenter).scale(scaleFactor);
  const auto translatedPoint = transform.transformPoint(viewPoint);
  std::cout << "(" << fromCenter.x << "x" << fromCenter.y << ")" << std::endl;
  std::cout << "(" << scaleFactor.x << "x" << scaleFactor.y << ")" << std::endl;
  std::cout << "(" << toCenter.x << "x" << toCenter.y << ")" << std::endl;
  std::cout << "(" << translatedPoint.x << "x" << translatedPoint.y << ")" << std::endl;

  return transform;
}

template <typename T, typename U>
const sf::Vector2<U> translatePointFromTo(const sf::Rect<T> from, const sf::Rect<U> to, const sf::Vector2<T> point) {
  const sf::Vector2f r1(from.left, from.top);
  const sf::Vector2f r2(to.left, to.top);

  const sf::Vector2f r1c(getCenter(from));
  const sf::Vector2f r2c(getCenter(to));

  const sf::Vector2f scaleFactor(to.width / from.width, -to.height / from.height);

  const sf::Vector2<U> translatedPoint(
//     (scaleFactor.x * (point.x + r1.x - r2.x)), // + r2c.x,
//    -(scaleFactor.y * (point.y + r1.y - r2.y)) // + r2c.y
     (scaleFactor.x * (point.x + r1.x - r2.x)),// + r2c.x,
    -(scaleFactor.y * (point.y + r1.y - r2.y)) // + r2c.y
  );
  std::cout << "(x,y)" << std::endl;
  std::cout << "(" << r1.x << "," << r1.y << ")" << "(" << r1c.x << "," << r1c.y << ")" << std::endl;
  std::cout << "(" << r2.x << "," << r2.y << ")" << "(" << r2c.x << "," << r2c.y << ")" << std::endl;
  std::cout << "(" << scaleFactor.x << "," << scaleFactor.y << ")" << std::endl;
  std::cout << "(" << translatedPoint.x << "," << translatedPoint.y << ")" << std::endl;
  return translatedPoint;
}


template <typename T>
void updateViewTexture(sf::Uint8* pixels, sf::Texture& texture, const sf::Rect<T> view) {
  const auto textureSize = texture.getSize();
  sf::Vector2u ti;
  sf::Vector2<T> c(view.left, view.top);
  const sf::Vector2<T> cInc(view.width / textureSize.x, view.height / textureSize.y);
  unsigned int i = 0;
  for(ti.y = 0, c.y = view.top; ti.y < textureSize.y; ++ti.y, c.y += cInc.y) {
    for(ti.x = 0, c.x = view.left; ti.x < textureSize.x; ++ti.x, c.x += cInc.x, i += 4) {
      const auto count = mandelbrot(c, 255);
      const sf::Color color(count*8, count*8, count*8);
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
  return sf::Vector2f(
    r.left + r.width / 2,
    r.top + r.height / 2
  );
}

void mouseCoordinates(const sf::Event& event, const sf::Transform& transformFromTextureToView, const sf::FloatRect& view, const sf::Texture& texture) {
  const sf::Vector2u mm(event.mouseMove.x, event.mouseMove.y);
  const auto textureSize = texture.getSize();
  const auto textureRect = sf::FloatRect(0, 0, textureSize.x, textureSize.y);
  const auto newRect = transformFromTextureToView.transformRect(textureRect);
  const auto vc = transformFromTextureToView.transformPoint(sf::Vector2f(mm.x, mm.y));
  const sf::Vector2f newViewDimensions(view.width / 2, view.height / 2);
  std::cout << "Mouse Center: (" << mm.x << "," << mm.y << ") " << "(" << textureRect.width << "x" << textureRect.height << ") ";
  std::cout << "View Center: (" << vc.x << "," << vc.y << ") " << "(" << view.width << "x" << view.height << ") ";
  std::cout << "New View Dimensions: (" << vc.x << "," << vc.y << ") " << "(" << newViewDimensions.x << "x" << newViewDimensions.y << ") ";
  std::cout << "New Rect: (" << newRect.left << "," << newRect.top << ")" << "(" << newRect.width << "x" << newRect.height << ") ";
  std::cout << std::endl;
}
