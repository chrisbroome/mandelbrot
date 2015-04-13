#ifndef POINT_H
#define POINT_H

template <typename T = int>
struct Point {
  T x;
  T y;
  Point(const Point<T> p) : x(p.x), y(p.y) {}
  Point(const T & xi, const T & yi) : x(xi), y(yi) { }
  Point() : x(0), y(0) { }

  inline Point<T> Add(const Point<T>& p) const { return Point(x + p.x, y + p.y); }
  inline Point<T> Sub(const Point<T>& p) const { return Point(x - p.x, y - p.y); }
  inline Point<T> Mul(const Point<T>& p) const { return Point(x * p.x, y * p.y); }
  inline Point<T> Div(const Point<T>& p) const { return Point(x / p.x, y / p.y); }
};

#endif
