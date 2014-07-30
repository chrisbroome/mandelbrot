#include <SFML/Graphics.hpp>

unsigned int mandelbrot(const sf::Vector2<double> c, const unsigned int numIterations);

int main() {

  sf::RenderWindow window(sf::VideoMode(640, 480), "Mandelbrot Set Viewer");

  sf::Texture texture; if (!texture.create(640, 480)) return -1;

  sf::Sprite sprite(texture);

  sf::Vector2u pi;
  const sf::Vector2u textureSize = texture.getSize();
  sf::Uint8 pixels[textureSize.x * textureSize.y * 4];

  const sf::Vector2<double> viewPosition(-2, -2);
  const sf::Vector2<double> viewSize(4, 4);
  const sf::Rect<double> view(viewPosition, viewSize);

  sf::Vector2<double> c(viewPosition);
  sf::Vector2<double> cInc(viewSize.x / textureSize.x, viewSize.y / textureSize.y);

  // double aInc = view.width / textureSize.x;
  // double bInc = view.height / textureSize.y;

  unsigned int i = 0;
  for(pi.y = 0, c.y = viewPosition.y; pi.y < textureSize.y; ++pi.y, c.y += cInc.y) {
    for(pi.x = 0, c.x = viewPosition.x; pi.x < textureSize.x; ++pi.x, c.x += cInc.x, i += 4) {
      auto count = mandelbrot(c, 255);
      sf::Color color(count, count, count);
      pixels[i] = color.r;
      pixels[i+1] = color.g;
      pixels[i+2] = color.b;
      pixels[i+3] = color.a;
    }
  }

  // while window is open
  while(window.isOpen()) {

    sf::Event event;
    while(window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
    window.clear(sf::Color::White);

    texture.update(pixels);

    window.draw(sprite);
    window.display();
  }

  return 0;
}

unsigned int mandelbrot(const sf::Vector2<double> c, const unsigned int numIterations)
{
  unsigned int count = 0;
  sf::Vector2<double> z(0, 0);
//  sf::Vector2<double> t;

  if (c.x < -2 || c.x > 2 || c.y < -2 || c.y > 2) return 0;
  for (; count < numIterations; ++count)
  {
    if (z.x*z.x + z.y*z.y > 4) return count;

    z = sf::Vector2<double>(z.x*z.x - z.y*z.y + c.x, 2 * z.x*z.y + c.y);
//    z.x = t.x * t.x - t.y * t.y + c.x; // x^2 - y^2 + a
//    z.y = 2 * t.x * t.y + c.y;         // 2xy + b
  }
  return z.x*z.x + z.y*z.y < 4 ? 0 : count;
}
