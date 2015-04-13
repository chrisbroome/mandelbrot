#include <stdio.h>

#include "types.h"

#include "state-serializer.h"

static Rect<ldouble> StateSerializer::LoadValues( const char * filename ) {
  FILE* fp = fopen(filename, "r");
  Rect<ldouble> rect
  if (fp) {
    fscanf(fp, "%80.78lf %80.78lf %80.78lf %80.78lf", &(rect.p1.x), &(rect.p1.y), &(rect.p2.x), &(rect.p2.y));
    fclose(fp);
  }
  return rect;
}

static void StateSerializer::SaveValues(const char * filename, const Rect<ldouble>& rect) {
  FILE* fp = fopen(filename, "w");
  if (!fp) return;
  fprintf(fp, "%80.78lf\n%80.78lf\n%80.78lf\n%80.78lf\n", rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);
  fclose(fp);
}
