#include "transforms.h"

bool computeMandelbrotPixels(
  uint8_t* out,
  sf::Vector2u size,
  sf::Rect<long double> view,
  const std::vector<sf::Color>& palette,
  uint64_t myJobId,
  const std::atomic<uint64_t>& currentJobId
) {
  if (palette.empty() || size.x == 0 || size.y == 0) return true;

  const auto numIterations = static_cast<unsigned int>(palette.size() - 1);
  const sf::Vector2<long double> cInc(
    view.size.x / static_cast<long double>(size.x),
    view.size.y / static_cast<long double>(size.y)
  );

  long double cy = view.position.y;
  for (unsigned y = 0; y < size.y; ++y, cy += cInc.y) {
    if (currentJobId.load(std::memory_order_relaxed) != myJobId) return false;

    uint8_t* row = out + static_cast<size_t>(y) * size.x * 4;
    long double cx = view.position.x;
    for (unsigned x = 0; x < size.x; ++x, cx += cInc.x) {
      const auto count = mandelbrot(sf::Vector2<long double>(cx, cy), numIterations);
      const auto& color = palette[count];
      row[x * 4 + 0] = color.r;
      row[x * 4 + 1] = color.g;
      row[x * 4 + 2] = color.b;
      row[x * 4 + 3] = color.a;
    }
  }
  return true;
}
