#ifndef RECT_H
#define RECT_H

#include "point.h"

template <typename T = int>
struct Rect {
  Point<T> p1;
  Point<T> p2;
  Rect( const Point<T> & p1i, const Point<T> & p2i ) : p1(p1i), p2(p2i) { Init(); }
  Rect() { Init(); }

  inline T Width() const { return p2.x - p1.x; }
  inline T Height() const { return p2.y - p1.y; }
  inline T HalfWidth() const { return Width() / 2; }
  inline T HalfHeight() const { return Height() / 2; }
  inline Point<T> Center() const { return p1.Add(p2).Div(Point(2, 2)); }
  inline Point<T> Dimensions() const { return p2.Sub(p1); }
  inline Point<T> HalfDimensions() const { return Dimensions().Div(Point(2, 2)); }

  inline Point<T> ScaleFactorFor(Rect<T> rect) const {
    return Dimensions().Div(rec.Dimensions());
  }

  void Normalize ()
  {
    if ( p1.x > p2.x )
      Swap ( p1.x, p2.x );
    if ( p1.y > p2.y )
      Swap ( p1.y, p2.y );
  }

protected:

  void Init()
  {
    // Normalize ( );
  }

};

#endif
