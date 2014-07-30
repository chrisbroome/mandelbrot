#ifndef OLD_LOAD_SAVE_H
#define OLD_LOAD_SAVE_H
#include <iostream>
#include <fstream>
#include <iomanip>

#include "types.h"

bool  LoadValues  ( const char * filename, ldouble & amin, ldouble & bmin, ldouble & amax, ldouble & bmax );
bool  LoadValues  ( const char * filename, Rect<ldouble> & rect );
void  SaveValues  ( const char * filename, ldouble amin, ldouble bmin, ldouble amax, ldouble bmax );
void  SaveValues  ( const char * filename, Rect<ldouble> rect );

bool LoadValues ( const char * filename, Rect<ldouble> & rect )
{
  ifstream  instr ( filename );
  if ( !instr )
    return false;
  else
  {
    instr >> rect.p1.x >> rect.p1.y >> rect.p2.x >> rect.p2.y;
    return true;
  }
}

bool LoadValues ( const char * filename, ldouble & x1, ldouble & y1, ldouble & x2, ldouble & y2 )
{
  ifstream  instr ( filename );
  if ( !instr )
    return false;
  else
  {
    instr >> x1 >> y1 >> x1 >> y2;
    return true;
  }
}

void SaveValues ( const char * filename, ldouble x1, ldouble y1, ldouble x2, ldouble y2 )
{
  FILE * fp;
  fp = fopen ( filename, "wt" );
  fprintf ( fp, "%80.78lf\n%80.78lf\n%80.78lf\n%80.78lf\n", x1, y1, x2, y2 );
  fclose ( fp );
}

void SaveValues ( const char * filename, Rect<ldouble> rect )
{
  FILE * fp;
  fp = fopen ( filename, "wt" );
  fprintf ( fp, "%80.78lf\n%80.78lf\n%80.78lf\n%80.78lf\n", rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y  );
  fclose ( fp );
}

#endif
