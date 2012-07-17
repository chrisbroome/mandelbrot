/*
	utilities.h
	Copyright 2002 Chris Broome
	All rights reserved
*/

#ifndef UTILITIES_H
#define UTILITIES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

const int VK_0 = 0x30;
const int VK_1 = 0x31;
const int VK_2 = 0x32;
const int VK_3 = 0x33;
const int VK_4 = 0x34;
const int VK_5 = 0x35;
const int VK_6 = 0x36;
const int VK_7 = 0x37;
const int VK_8 = 0x38;
const int VK_9 = 0x39;

const int VK_A = 0x41;
const int VK_B = 0x42;
const int VK_C = 0x43;
const int VK_D = 0x44;
const int VK_E = 0x45;
const int VK_F = 0x46;
const int VK_G = 0x47;
const int VK_H = 0x48;
const int VK_I = 0x49;
const int VK_J = 0x4a;
const int VK_K = 0x4b;
const int VK_L = 0x4c;
const int VK_M = 0x4d;
const int VK_N = 0x4e;
const int VK_O = 0x4f;
const int VK_P = 0x50;
const int VK_Q = 0x51;
const int VK_R = 0x52;
const int VK_S = 0x53;
const int VK_T = 0x54;
const int VK_U = 0x55;
const int VK_V = 0x56;
const int VK_W = 0x57;
const int VK_X = 0x58;
const int VK_Y = 0x59;
const int VK_Z = 0x5a;

inline BOOL KEY_DOWN	( int vKey ) { return ( GetAsyncKeyState ( vKey ) & 0x8000 ? true : false ); }
inline BOOL KEY_UP		( int vKey ) { return ( GetAsyncKeyState ( vKey ) & 0x8000 ? false : true ); }

inline BYTE RED			( COLORREF c )	{ return (BYTE)c; }
inline BYTE GREEN		( COLORREF c )	{ return (BYTE)((c & 0x0000ff00) >>  8); }
inline BYTE BLUE		( COLORREF c )	{ return (BYTE)((c & 0x00ff0000) >> 16); }


template <typename T>
Swap ( T & x1, T & x2 )
{
	T temp = x1;
	x1 = x2;
	x2 = temp;
}

#endif
