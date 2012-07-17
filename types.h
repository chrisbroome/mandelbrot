/*
	types.h
	Copyright 2001 Chris Broome
	All rights reserved
*/

#ifndef TYPES_H
#define TYPES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
#ifndef	DOUBLE
#define	DOUBLE	double
#endif
*/
#ifndef	LDOUBLE
#define	LDOUBLE	long double
#endif

#include "utilities.h"

template <typename T = UINT>
struct Point
{
	T x, y;
	Point ( const T & xi, const T & yi ) : x(xi), y(yi) { }
	Point ( ) : x(0), y(0) { }

};

template <typename T = UINT>
struct Rect
{
	Point<T> p1, p2;
	Rect ( const Point<T> & p1i, const Point<T> & p2i ) : p1(p1i), p2(p2i) { Init(); }
	Rect ( ) { Init(); }

	inline T Width ( void ) { return p2.x - p1.x; }
	inline T Height ( void ) { return p2.y - p1.y; }
	inline Point<T> Center ( void ) { return Point<T> ( (p1.x + p2.x) / 2, (p1.y + p2.y) / 2); }
	inline T HalfWidth ( void ) { return Width()/2; }
	inline T HalfHeight ( void ) { return Height()/2; }

	void	Normalize ( void )
	{
		if ( p1.x > p2.x )
			Swap ( p1.x, p2.x );
		if ( p1.y > p2.y )
			Swap ( p1.y, p2.y );
	}

protected:

	void Init ( )
	{
		// Normalize ( );
	}

};

#endif
