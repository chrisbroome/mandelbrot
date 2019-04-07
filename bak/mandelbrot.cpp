// includes ///////////////////////////////////////////////
#include <cmath>   // sin
#include <cstring> //
#include <ctime>
#include <fstream>  //
#include <iomanip>  //
#include <iostream> // C++ stuff

#include <SFML/Window.hpp>

#include "app-params.h"
#include "app.h"

#include "point.h"
#include "rect.h"
#include "state-serializer.h"
#include "utilities.h" // important utilities

using namespace std;

// CONSTANTS //////////////////////////////////////////////

// enumerated iterated function types
enum ITERATED_FUNCTION { mandlebrot = 1, julia, newton1 };

// constants for windows
const CHAR* WINDOW_CLASS_NAME = "WINXCLASS"; // class name
const UINT SCREEN_BPP = 8;                   // number of bits per pixel (2 to this power gives the number of colors)

// PROTOTYPES /////////////////////////////////////////////

void Welcome(LPDIRECTDRAWSURFACE lpdds);

BYTE Mandlebrot(const LDOUBLE& a, const LDOUBLE& b, const BYTE& numiterations);
BYTE Julia(const LDOUBLE& xparam, const LDOUBLE& yparam, const LDOUBLE& a, const LDOUBLE& b, const BYTE& numiterations);

void Gradient(COLORREF c1, COLORREF c2, UINT start, UINT finish);
void Gradient(PALETTEENTRY c1, PALETTEENTRY c2, UINT start, UINT finish);
void Gradient(PALETTEENTRY colorarray[], WORD numcolors, UINT start, UINT finish);

// GLOBALS ////////////////////////////////////////////////

// do not change after initialization!!!
Rect<UINT> Window(Point<UINT>(0, 0), Point<UINT>(640, 480));
Rect<UINT> Screen(Point<UINT>(0, 0), Point<UINT>(640, 480));

// change all you want after initialization
const char* default_filename = "mandvals.txt";

Rect<UINT> View(Window);

Rect<LDOUBLE> World(Point<LDOUBLE>(-2, 2), Point<LDOUBLE>(-2, 2));

int main() {

  auto wd = Window.dimensions();

  App app(AppParams());

sf:
  Window window(sf::VideoMode(wd.x, wd.y), "Mandelbrot Set Viewer");

  app.Init();
  while (window.isOpen()) {

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
    app.Main();
  }

  app.ShutDown();

  return 0;
}

// WINMAIN ////////////////////////////////////////////////

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow) {

  bool done = false; // used in the while loop.  set to true if finished with program
  WNDCLASS winclass; // this will hold the class we create
  HWND hwnd;         // generic window handle
  MSG msg;           // generic message
                     //  HMENU    hMenu;      // generic menu
                     //  HDC      hdc;      // generic dc
                     //  PAINTSTRUCT  ps;        // generic paintstruct

  // first fill in the window class stucture
  winclass.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  winclass.lpfnWndProc = WindowProc;
  winclass.cbClsExtra = 0;
  winclass.cbWndExtra = 0;
  winclass.hInstance = hinstance;
  winclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  winclass.hCursor = LoadCursor(NULL, IDC_CROSS);
  winclass.hbrBackground = NULL; // GetStockObject ( BLACK_BRUSH );
  winclass.lpszMenuName = NULL;  // MAKEINTRESOURCE ( IDR_MENU1 );
  winclass.lpszClassName = WINDOW_CLASS_NAME;

  // register the window class
  if (!RegisterClass(&winclass)) return (0);

  // create the window, note the use of WS_POPUP
  if (!(hwnd = CreateWindow(WINDOW_CLASS_NAME,                     // class
                            "WinX Game Console",                   // title
                            WS_POPUP | WS_VISIBLE | WS_EX_TOPMOST, // properties of the window
                            0, 0,                                  // x,y
                            Window.Width(), Window.Height(),       // width, height
                            NULL,                                  // handle to parent
                            NULL,                                  // handle to menu
                            hinstance,                             // instance
                            NULL)))                                // creation parms
    return (0);

  // save the window handle and instance in a global
  main_window_handle = hwnd;
  main_instance = hinstance;

  //  perform all game console specific initialization
  Game_Init();

  // enter main event loop
  while (!done) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      // test if this is a quit
      if (msg.message == WM_QUIT) done = true;

      // translate any accelerator keys
      TranslateMessage(&msg);

      // send the message to the window proc
      DispatchMessage(&msg);
    } // end if

    // main game processing goes here
    Game_Main();

  } // end while

  // shutdown game and release all resources
  Game_Shutdown();

  // return to Windows like this
  return msg.wParam;
} // end WinMain

inline ULONG FastFact(const ULONG n) {
  switch (n) {
  case 2L:
    return 2L;
  case 3L:
    return 6L;
  case 4L:
    return 24L;
  case 5L:
    return 120L;
  case 6L:
    return 720L;
  case 7L:
    return 5040L;
  case 8L:
    return 40320L;
  case 9L:
    return 362880L;
  case 10L:
    return 3628800L;
  case 11L:
    return 39916800L;
  case 12L:
    return 479001600L;
  default:
    return 1L;
  }
}

inline ULONG NchooseK(const ULONG n, const ULONG k) { return (FastFact(n) / FastFact(k)) / FastFact(n - k); }

LDOUBLE QuickPow(LDOUBLE base, ULONG exponent) {
  LDOUBLE a = 1.0;
  LDOUBLE b = base;
  ULONG e = exponent;
  // inv 1:base ˆ exponent = a * b ˆ e and e >= 0
  while (e > 0) {
    // inv 2: base ˆ exponent = a * b ˆ e and e >= 0
    if (e % 2 != 0) a = a * b;
    b = b * b;
    e = e / 2;
    // inv 3: base ˆ exponent = a * b ˆ e and e >= 0
  } // inv 4: base ˆ exponent = a and e == 0
  return a;
}

VOID ComplexBinomial(const ULONG power, const LDOUBLE x, const LDOUBLE y, LDOUBLE& realAns, LDOUBLE& imagAns) {
  const ULONG n = power;
  const ULONG nOver2 = n / 2;

  ULONG twoK, twoKPlus1;
  LONG minus1 = -1;

  LDOUBLE xsum = 0, ysum = 0;

  for (ULONG k = 0; k <= nOver2; ++k) {
    twoK = 2 * k;
    twoKPlus1 = twoK + 1;
    minus1 = -1 * minus1;
    ysum += minus1 * NchooseK(n, twoK) * pow(x, twoK) * pow(y, n - twoK);
    if (twoKPlus1 <= n) xsum += minus1 * NchooseK(n, twoKPlus1) * pow(x, twoKPlus1) * pow(y, n - twoKPlus1);
  }
  realAns = xsum;
  imagAns = ysum;
}

BYTE Mandlebrot(const LDOUBLE& a, const LDOUBLE& b, const BYTE& numiterations) {
  LDOUBLE x = 0, y = 0, xtemp, ytemp;

  WORD count = 0;

  if (a < -2 || a > 2 || b < -2 || b > 2) return 0;
  for (; count < numiterations; ++count) {
    xtemp = x;
    ytemp = y;

    if (x * x + y * y > 4) return count;

    //    x = xtemp*xtemp - ytemp*ytemp + a;  // x^2 - y^2 + a
    //    y = 2*xtemp*ytemp + b;        // 2xy + b

    //    x = 5*xtemp*ytemp*ytemp*ytemp*ytemp - 10*xtemp*xtemp*xtemp*ytemp*ytemp + xtemp*xtemp*xtemp*xtemp*xtemp + a;
    //    y = ytemp*ytemp*ytemp*ytemp*ytemp - 10*xtemp*xtemp*ytemp*ytemp*ytemp + 5*xtemp*xtemp*xtemp*xtemp*ytemp + b;

    //    x = 5*xtemp*QuickPow(ytemp,4) - 10*QuickPow(xtemp,3)*QuickPow(ytemp,2) + QuickPow(xtemp,5) + a;
    //    y = QuickPow(ytemp,5) - 10*QuickPow(xtemp,2)*QuickPow(ytemp,3) + 5*QuickPow(xtemp,4)*ytemp + b;
    //    ComplexBinomial( 5, xtemp, ytemp, x, y );
    //    x += a;
    //    y += b;

    x = ytemp * ytemp * ytemp * ytemp * ytemp * ytemp - 15 * xtemp * xtemp * ytemp * ytemp * ytemp * ytemp +
        15 * ytemp * ytemp * xtemp * xtemp * xtemp * xtemp - xtemp * xtemp * xtemp * xtemp * xtemp * xtemp + a;
    y = 6 * xtemp * ytemp * ytemp * ytemp * ytemp * ytemp - 20 * xtemp * xtemp * xtemp * ytemp * ytemp * ytemp +
        6 * xtemp * xtemp * xtemp * xtemp * xtemp * ytemp + b;
  }
  if (x * x + y * y < 4)
    return 0;
  else
    return count;
}

BYTE Julia(const LDOUBLE& xparam, const LDOUBLE& yparam, const LDOUBLE& a, const LDOUBLE& b,
           const BYTE& numiterations) {
  LDOUBLE x = xparam, y = yparam, xtemp, ytemp;

  WORD count = 0;

  if (xparam < -2 || xparam > 2 || yparam < -2 || yparam > 2) return 0;
  for (; count < numiterations; ++count) {
    xtemp = x;
    ytemp = y;

    if (x * x + y * y > 4) return count;

    x = xtemp * xtemp - ytemp * ytemp + -a; // x^2 - y^2 + a
    y = 2 * xtemp * ytemp + b;              // 2xy + b
                                            //    x = xtemp*xtemp - ytemp*ytemp + a;  // x^2 - y^2 + a
                                            //    y = 2*xtemp*ytemp + b;        // 2xy + b
  }
  if (x * x + y * y < 4)
    return 0;
  else
    return count;
}

void Welcome(LPDIRECTDRAWSURFACE lpdds) {
  const UINT MAXLEN = 85;
  const UINT NUMLINES = 17;
  char message[NUMLINES][MAXLEN] = {
      "Mandlebrot and Julia Set Viewer",
      "",
      "Controls:",
      "Z lets you define a new window to zoom into by clicking and dragging the mouse",
      "UP, DOWN, LEFT, RIGHT arrows scroll the current view",
      "SPACEBAR resets the view to the default window [(-2,-2i),(2,2i)]",
      "ENTER refreshes the current view (also wipes out any orbit diagrams)",
      "M switches to Mandlebrot Set",
      "J switches to Julia Set (after getting the c value for z^2 + c)",
      "C chooses a point in the complex plane to plot in the orbit diagram",
      "O (the letter O, not the number 0) plots the orbit diagram of the specified point ",
      "     (warning... plotting the orbit in a zoomed view will cause unexpected results.",
      "     In order to plot the orbit of a point you choose from a zoomed view, you must",
      "     reset the view with SPACEBAR before pressing O to plot the orbit)",
      "ESCAPE exits the program",
      "",
      "Press SPACEBAR to start"};

  HDC hdc;
  lpdds->GetDC(&hdc);
  for (UINT i = 0; i < NUMLINES; ++i)
    TextOut(hdc, 0, i * 16, message[i], strlen(message[i]));

  lpdds->ReleaseDC(hdc);
}

void Gradient(COLORREF c1, COLORREF c2, UINT start, UINT finish) {
  WORD a;
  BYTE ri = RED(c1), gi = GREEN(c1), bi = BLUE(c1), rf = RED(c2), gf = GREEN(c2), bf = BLUE(c2);

  double rinc, ginc, binc;
  double r = 0, g = 0, b = 0;
  BYTE R, G, B;

  rinc = (double)(rf - ri) / abs(finish - start + 1);
  ginc = (double)(gf - gi) / abs(finish - start + 1);
  binc = (double)(bf - bi) / abs(finish - start + 1);

  R = ri;
  G = gi;
  B = bi;
  for (a = start; a <= finish; ++a) {
    r += rinc;
    g += ginc;
    b += binc;
    R = ri + (BYTE)r;
    G = gi + (BYTE)g;
    B = bi + (BYTE)b;
    palette[a].peRed = R;
    palette[a].peGreen = G;
    palette[a].peBlue = B;
    palette[a].peFlags = PC_NOCOLLAPSE;
  }
  lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, palette, &lpddpal, NULL);
  lpddsprimary->SetPalette(lpddpal);
}

void Gradient(PALETTEENTRY c1, PALETTEENTRY c2, UINT start, UINT finish) {
  WORD n;
  PALETTEENTRY i = c1, f = c2, cur;

  DOUBLE rinc, ginc, binc;
  DOUBLE r = 0, g = 0, b = 0;

  rinc = (DOUBLE)(f.peRed - i.peRed) / abs(finish - start + 1);
  ginc = (DOUBLE)(f.peGreen - i.peGreen) / abs(finish - start + 1);
  binc = (DOUBLE)(f.peBlue - i.peBlue) / abs(finish - start + 1);

  cur = i;
  for (n = start; n <= finish; ++n) {
    r += rinc;
    g += ginc;
    b += binc;
    cur.peRed = i.peRed + (BYTE)r;
    cur.peGreen = i.peGreen + (BYTE)g;
    cur.peBlue = i.peBlue + (BYTE)b;
    cur.peFlags = PC_NOCOLLAPSE;
    palette[n] = cur;
  }
  lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, palette, &lpddpal, NULL);
  //  lpddpal->SetEntries ( 0, start, finish - start + 1, palette );
  lpddsprimary->SetPalette(lpddpal);
}

void Gradient(PALETTEENTRY colorarray[], WORD numelements, UINT start, UINT finish) {
  UINT s = start, f = finish;

  if (numelements == 0) return;

  if (s == f) {
    palette[s] = colorarray[numelements / 2];
  } else {
    if (s > f)
      Swap(s, f);
    else {
      UINT numcolors = f - s + 1;
      DOUBLE inc = (DOUBLE)numcolors / (numelements - 1);
      s = start;
      f = s + (WORD)inc - 1;
      COLORREF c1, c2;
      for (WORD i = 0; i < numelements - 1; ++i) {
        c1 = RGB(colorarray[i].peRed, colorarray[i].peGreen, colorarray[i].peBlue);
        c2 = RGB(colorarray[i + 1].peRed, colorarray[i + 1].peGreen, colorarray[i + 1].peBlue);
        //        Gradient ( colorarray[i], colorarray[i+1], s, f );
        Gradient(c1, c2, s, f);
        s = f + 1;
        f += (WORD)inc;
      }
    }
  }
  lpdd->CreatePalette(DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, palette, &lpddpal, NULL);
  lpddsprimary->SetPalette(lpddpal);
}

Point TranslateCoordinates(Point p, Rect world, Rect view) {
  Point cw = world.Center();
  Point slope = p.Add(cw).Sub(view.Center());
  Point scaleFactor = world.ScaleFactorFor(view).Mul(1, -1);
  return scaleFactor.Mul(slope).Add(cw);
  //  return Point(
  //     sf.x * (p.x + cw.x - cv.x) + cw.x,
  //    -sf.y * (p.y + cw.y - cv.y) + cw.y
  //  );
}

Point TranslateMoreCoordinates(Point p, Rect world, Rect view) {
  Point p1;
  Point wd = world.Dimensions();
  Point vd = view.Dimensions();
  Point cw = world.Center();
  Point cv = view.Center();
  p1.x = vd.x + (+p.x * vd.x + wd.x * cw.x - wd.x * cv.x + cw.x * vd.x) / wd.x;
  p1.y = vd.y + (-p.y * vd.y + wd.y * cw.y - wd.y * cv.y + cw.y * vd.y) / wd.y;
  return p1;
}
