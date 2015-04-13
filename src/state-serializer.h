#ifndef STATE_SERIALIZER_H
#define STATE_SERIALIZER_H

#include "types.h"
#include "point.h"
#include "rect.h"

class StateSerializer {
public:
  static Rect<ldouble> LoadValues(const char* filename) const;
  static bool LoadValues(const char* filename, Rect<ldouble>& rect);
  static void SaveValues(const char * filename, const Rect<ldouble>& rect);
};

#endif
