#include "app.h"

int App::Init(void* parms) {
  // this function is where you do all the initialization for your game

  // create object and test for error
  if (DirectDrawCreate(NULL, &lpdd, NULL) != DD_OK) return 0;

  // set cooperation level to windowed mode normal
  if (lpdd->SetCooperativeLevel(main_window_handle,
                                DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_ALLOWREBOOT) != DD_OK)
    return 0;

  // set the display mode
  if (lpdd->SetDisplayMode(Screen.Width(), Screen.Height(), SCREEN_BPP) != DD_OK) return 0;

  // Create the primary surface
  memset(&ddsd, 0, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);

  // set the flags to validate both the capabilities
  // field and the backbuffer count field
  ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

  // we need to let dd know that we want a complex
  // flippable surface structure, set flags for that
  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

  // set the backbuffer count to 1
  ddsd.dwBackBufferCount = 1;

  // create the primary surface
  lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL);

  // query for the backbuffer or secondary surface
  // notice the use of ddscaps to indicate what
  // we are requesting
  ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

  // get the surface
  lpddsprimary->GetAttachedSurface(&ddscaps, &lpddsback);

  // allocate memory for the double buffer
  double_buffer = new UCHAR[Screen.Width() * Screen.Height()];
  if (!double_buffer) return (0);

  /*
    Red     ff   0   0
    Orange  ff  7f   0
    Yellow  ff  ff   0
    Y - G   7f  ff   0
    Green    0  ff   0
    Blue     0   0  ff
    Purple  ff   0  ff
    Pink    7f   0  7f
    Black    0   0   0
  */
  PALETTEENTRY grad[9] = {
      {0xff, 0x00, 0x00, PC_NOCOLLAPSE}, {0xff, 0x7f, 0x00, PC_NOCOLLAPSE}, {0xff, 0xff, 0x00, PC_NOCOLLAPSE},
      {0x7f, 0xff, 0x00, PC_NOCOLLAPSE}, {0x00, 0xff, 0x00, PC_NOCOLLAPSE}, {0x00, 0x00, 0xff, PC_NOCOLLAPSE},
      {0xff, 0x00, 0xff, PC_NOCOLLAPSE}, {0x7f, 0x00, 0x7f, PC_NOCOLLAPSE}, {0x00, 0x00, 0x00, PC_NOCOLLAPSE}};

  Gradient(grad, 9, 0, 255);
  //  StateSerializer::LoadValues ( default_filename, World );
  Welcome(lpddsprimary);

  // return success
  return 1;

} // end Game_Init

int App::Shutdown(void* parms) {
  // this function is where you shutdown your game and
  // release all resources that you allocated

  // first release the secondary surface
  if (lpddsback) lpddsback->Release();

  // now release the primary surface
  if (lpddsprimary) lpddsprimary->Release();

  // release the directdraw object
  if (lpdd) lpdd->Release();

  // free double buffer
  if (double_buffer) delete[] double_buffer;

  // return success
  return 1;
} // end Game_Shutdown

int App::Main(void* parms) {
  // this is the workhorse of your game it will be called continuously in real-time this is like main() in C
  // all the calls for you game go here!

  UCHAR *back_buffer = NULL, // used to draw
      *dest_ptr = NULL,      // used in line by line copy
          *src_ptr = NULL;   // " "

  static ITERATED_FUNCTION whichfunction = mandlebrot;

  static BYTE numits = 254;
  static BOOL update = false;
  static BOOL done = false;
  static BOOL display_stats = false;

  static Point<UINT> zoomspeed(80, 60);
  static Point<LDOUBLE> inc;
  static Point<LDOUBLE> orbitPoint;

  static char s[200];

  static Point<LDOUBLE> jparam(0, 0);

  if (KEY_DOWN(VK_C)) {
    POINT p1;
    HDC hdc;
    bool abortChooseC = false;
    lpddsprimary->GetDC(&hdc);
    while (!KEY_DOWN(VK_LBUTTON) && !abortChooseC) {
      GetCursorPos(&p1);
      orbitPoint.x = p1.x * (World.Width() / View.Width()) +
                     ((World.Center().x - View.Center().x) * World.Width()) / View.Width() + World.Center().x;
      orbitPoint.y = -(p1.y * (World.Height() / View.Height()) +
                       ((World.Center().y - View.Center().y) * World.Height()) / View.Height()) +
                     World.Center().y;
      sprintf(s, "x0 = %16.14lf + %16.14lfi", orbitPoint.x, orbitPoint.y);
      TextOut(hdc, 0, 16 * 1, s, strlen(s));
      if (KEY_DOWN(VK_ESCAPE)) {
        abortChooseC = true;
        while (KEY_DOWN(VK_ESCAPE))
          ;
      }
    }
    lpddsprimary->ReleaseDC(hdc);
  }
  if (KEY_DOWN(VK_Z)) {
    while (KEY_DOWN(VK_Z))
      ;

    POINT p1, p2;
    HDC hdc;
    bool abortZoom = false;
    lpddsprimary->GetDC(&hdc);
    Rect<LDOUBLE> zoom;

    sprintf(s, "Zooming Function");
    TextOut(hdc, 0, 0, s, strlen(s));

    while (!KEY_DOWN(VK_LBUTTON) && !abortZoom) {
      GetCursorPos(&p1);
      zoom.p1.x = p1.x * (World.Width() / View.Width()) +
                  ((World.Center().x - View.Center().x) * World.Width()) / View.Width() + World.Center().x;
      zoom.p1.y = -(p1.y * (World.Height() / View.Height()) +
                    ((World.Center().y - View.Center().y) * World.Height()) / View.Height()) +
                  World.Center().y;
      sprintf(s, "Mouse Position: (%4d,%4d) c = %16.14lf + %16.14lfi", p1.x, p1.y, zoom.p1.x, zoom.p1.y);
      TextOut(hdc, 0, 16 * 1, s, strlen(s));
      if (KEY_DOWN(VK_ESCAPE)) abortZoom = true;
    }
    //    RECT r;
    while (KEY_DOWN(VK_LBUTTON) && !abortZoom) {
      GetCursorPos(&p2);
      /*
            r.top = p1.x;
            r.left = p1.y;
            r.bottom = p2.x;
            r.right = p2.y;
      */
      zoom.p2.x = p2.x * (World.Width() / View.Width()) +
                  ((World.Center().x - View.Center().x) * World.Width()) / View.Width() + World.Center().x;
      zoom.p2.y = -(p2.y * (World.Height() / View.Height()) +
                    ((World.Center().y - View.Center().y) * World.Height()) / View.Height()) +
                  World.Center().y;

      RECT r;

      r.left = p1.x;
      r.top = p1.y;
      r.right = p2.x;
      r.bottom = p2.y;

      //      InvertRect( hdc, &r );
      sprintf(s, "Mouse Position: (%4d,%4d) c = %16.14lf + %16.14lfi", p2.x, p2.y, zoom.p2.x, zoom.p2.y);
      TextOut(hdc, 0, 16 * 1, s, strlen(s));
      if (KEY_DOWN(VK_ESCAPE)) abortZoom = true;
    }

    if (!abortZoom) {
      zoom.Normalize();
      //      View.Normalize();
      World = zoom;
      sprintf(s, "New coordinates: [(%16.14lf,%16.14lf),(%16.14lf,%16.14lf)]", World.p1.x, World.p1.y, World.p2.x,
              World.p2.y);
      TextOut(hdc, 0, 16 * 2, s, strlen(s));
      //      MessageBox ( main_window_handle, s, "New coordinates", 0 );
      update = true;
    } else {
      while (KEY_DOWN(VK_ESCAPE))
        ;
    }
    lpddsprimary->ReleaseDC(hdc);
  }
  /*
    if ( KEY_DOWN ( VK_N ) )
    {
      whichfunction = newton1;
      update = true;
    }
  */
  if (KEY_DOWN(VK_M)) {
    whichfunction = mandlebrot;
    update = true;
  }
  if (KEY_DOWN(VK_J)) {
    while (KEY_DOWN(VK_J))
      ;
    POINT p1;
    HDC hdc;
    lpddsprimary->GetDC(&hdc);

    sprintf(s, "Julia Set for:");
    TextOut(hdc, 0, 0, s, strlen(s));

    while (!KEY_DOWN(VK_LBUTTON)) {
      GetCursorPos(&p1);
      jparam.x = p1.x * (World.Width() / View.Width()) +
                 ((World.Center().x - View.Center().x) * World.Width()) / View.Width() + World.Center().x;
      jparam.y = -(p1.y * (World.Height() / View.Height()) +
                   ((World.Center().y - View.Center().y) * World.Height()) / View.Height()) +
                 World.Center().y;
      sprintf(s, "c = %16.14lf + %16.14lfi", jparam.x, jparam.y);
      TextOut(hdc, 0, 16 * 1, s, strlen(s));
    }
    while (KEY_DOWN(VK_LBUTTON)) {
      GetCursorPos(&p1);
      jparam.x = p1.x * (World.Width() / View.Width()) +
                 ((World.Center().x - View.Center().x) * World.Width()) / View.Width() + World.Center().x;
      jparam.y = -(p1.y * (World.Height() / View.Height()) +
                   ((World.Center().y - View.Center().y) * World.Height()) / View.Height()) +
                 World.Center().y;
      sprintf(s, "c = %16.14lf + %16.14lfi", jparam.x, jparam.y);
      TextOut(hdc, 0, 16 * 1, s, strlen(s));
    }

    lpddsprimary->ReleaseDC(hdc);

    display_stats = true;
    update = true;
    whichfunction = julia;
  }
  if (KEY_DOWN(VK_O)) {
    while (KEY_DOWN(VK_O) || KEY_DOWN(VK_LBUTTON))
      ;
    Point<ULONG> p1(Screen.Center());
    HDC hdc;
    lpddsprimary->GetDC(&hdc);
    sprintf(s, "Orbit Analysis for the point %16.14lfi + %16.14lfi: ", orbitPoint.x, orbitPoint.y);
    TextOut(hdc, 0, 0, s, strlen(s));

    SelectObject(
        hdc, CreatePen(PS_SOLID, 0, RGB(64 + rand() % (256 - 64), 64 + rand() % (256 - 64), 64 + rand() % (256 - 64))));

    LDOUBLE x = 0, y = 0, a = orbitPoint.x, b = orbitPoint.y, xtemp, ytemp;

    WORD count = 0, numiterations = 256;

    MoveToEx(hdc, p1.x, p1.y, NULL);
    while ((x > -2 && x < 2 && y > -2 && y < 2) && count < numiterations) {
      p1.x = (View.Width()) + (+x * View.Width() + World.Width() * World.Center().x - World.Width() * View.Center().x +
                               World.Center().x * View.Width()) /
                                  World.Width();
      p1.y = (View.Height()) + (-y * View.Height() + World.Height() * World.Center().y -
                                World.Height() * View.Center().y + World.Center().y * View.Height()) /
                                   World.Height();
      LineTo(hdc, p1.x, p1.y);
      sprintf(s, "x%-3d = %16.14lf + %16.14lfi", count, x, y, p1.x, p1.y);
      TextOut(hdc, 0, 16 * 2, s, strlen(s));

      xtemp = x;
      ytemp = y;

      x = xtemp * xtemp - ytemp * ytemp + a; // x^2 - y^2 + a
      y = 2 * xtemp * ytemp + b;             // 2xy + b

      ++count;
    }
    lpddsprimary->ReleaseDC(hdc);
  }
  if (KEY_DOWN(VK_RETURN)) {
    update = true;
  }
  if (KEY_DOWN(VK_ESCAPE)) {
    done = true;
    PostQuitMessage(0);
  }
  if (KEY_DOWN(VK_HOME)) {
    ++numits;
    update = true;
  }
  if (KEY_DOWN(VK_END)) {
    --numits;
    update = true;
  }
  if (KEY_DOWN(VK_DOWN)) {
    World.p1.y += inc.y * zoomspeed.y;
    World.p2.y += inc.y * zoomspeed.y;
    update = true;
  }
  if (KEY_DOWN(VK_UP)) {
    World.p1.y -= inc.y * zoomspeed.y;
    World.p2.y -= inc.y * zoomspeed.y;
    update = true;
  }
  if (KEY_DOWN(VK_RIGHT)) {
    World.p1.x -= inc.x * zoomspeed.x;
    World.p2.x -= inc.x * zoomspeed.x;
    update = true;
  }
  if (KEY_DOWN(VK_LEFT)) {
    World.p1.x += inc.x * zoomspeed.x;
    World.p2.x += inc.x * zoomspeed.x;
    update = true;
  }
  if (KEY_DOWN(VK_PRIOR)) // zoom in
  {
    World.p1.x += inc.x * zoomspeed.x;
    World.p2.x -= inc.x * zoomspeed.x;
    World.p1.y += inc.y * zoomspeed.y;
    World.p2.y -= inc.y * zoomspeed.y;
    update = true;
  }
  if (KEY_DOWN(VK_NEXT)) // zoom out
  {
    World.p1.x -= inc.x * zoomspeed.x;
    World.p2.x += inc.x * zoomspeed.x;
    World.p1.y -= inc.y * zoomspeed.y;
    World.p2.y += inc.y * zoomspeed.y;
    update = true;
  }
  if (KEY_DOWN(VK_P)) {
    while (KEY_DOWN(VK_P))
      ;

    HDC hdc;
    lpddsprimary->GetDC(&hdc);
    sprintf(s, "Bitmap saving not implemented yet");
    TextOut(hdc, 0, 16, s, strlen(s));
    lpddsprimary->ReleaseDC(hdc);
  }
  if (KEY_DOWN(VK_L)) // load mandlebrot zoom rectangle values
  {
    while (KEY_DOWN(VK_S))
      ;
    StateSerializer::LoadValues(default_filename, World);
    World.Normalize();
    update = true;
  }
  if (KEY_DOWN(VK_S)) // save mandlebrot zoom rectangle values
  {
    while (KEY_DOWN(VK_S))
      ;
    StateSerializer::SaveValues(default_filename, World);
  }
  if (KEY_DOWN(VK_SPACE)) // reset to default view
  {
    World.p1.x = World.p1.y = -2.0;
    World.p2.x = World.p2.y = 2.0;
    update = true;
  }
  if (KEY_DOWN(VK_LSHIFT)) {
    display_stats ? display_stats = false : display_stats = true;
    update = true;
    while (KEY_DOWN(VK_LSHIFT)) {
    }
  }
  if (KEY_DOWN(VK_SUBTRACT)) {
    zoomspeed.x -= 4;
    zoomspeed.y -= 3;
    while (KEY_DOWN(VK_SUBTRACT)) {
    }
  }
  if (KEY_DOWN(VK_ADD)) {
    zoomspeed.x += 4;
    zoomspeed.y += 3;
    while (KEY_DOWN(VK_ADD)) {
    }
  }
  if (update) {
    // erase secondary back buffer
    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    // lock the secondary surface
    lpddsback->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

    // get video pointer to secondary surface
    back_buffer = (UCHAR*)ddsd.lpSurface;

    // clear out the back_buffer
    if (ddsd.lPitch == Screen.Width()) memset(back_buffer, 0, Screen.Width() * Screen.Height());
    // non-linear memory
    else {
      // make copy of video pointer
      dest_ptr = back_buffer;

      // clear out memory one line at a time
      for (int y = 0; y < Screen.Height(); ++y) {
        // clear next line
        memset(dest_ptr, 0, Screen.Width());
        // advance pointer to next line
        dest_ptr += ddsd.lPitch;
      } // end for y

    } // end else

    // perform game logic...

    // draw the next frame into the secondary back buffer

    HDC hdc;
    lpddsprimary->GetDC(&hdc);
    sprintf(s, "Updating...");
    TextOut(hdc, 0, View.Height() - 16, s, strlen(s));
    lpddsprimary->ReleaseDC(hdc);

    LDOUBLE a;
    LDOUBLE b = World.p1.y;

    time_t last;
    time_t now;

    time(&now);
    last = now - 1;

    inc.x = (World.p2.x - World.p1.x) / View.Width();
    inc.y = (World.p2.y - World.p1.y) / View.Height();

    for (int y = View.Height() - 1; y >= 0; --y) {
      a = World.p1.x;
      for (int x = 0; x < View.Width(); ++x) {
        switch (whichfunction) {
        case julia:
          back_buffer[x + y * ddsd.lPitch] = Julia(a, b, jparam.x, jparam.y, numits);
          break;
        case mandlebrot:
          back_buffer[x + y * ddsd.lPitch] = Mandlebrot(a, b, numits);
          break;
          //          case newton1:
          //            back_buffer [ x + y * ddsd.lPitch ] =  Newton1  ( a, b, numits );
          break;
        default:
          back_buffer[x + y * ddsd.lPitch] = 0;
          break;
        }
        a += inc.x;
        if (last < now) {
          lpddsprimary->GetDC(&hdc);
          sprintf(s, "Updating: %8.5lf%%",
                  100 * LDOUBLE(x + (View.Height() - y) * View.Width()) / (View.Width() * View.Height()));
          TextOut(hdc, 0, View.Height() - 16, s, strlen(s));
          lpddsprimary->ReleaseDC(hdc);
          last = now;
        }
        time(&now);
      }
      b += inc.y;
    }
    update = false;
    // linear memory

    // unlock secondary buffer
    lpddsback->Unlock(back_buffer);

    /*
        if ( display_stats )
        {
          HDC hdc;
          lpddsback->GetDC ( & hdc );
          TextOut ( hdc, 0, 0, s, strlen(s) );
          lpddsback->ReleaseDC ( hdc );
          display_stats = false;
        }
    */
    // flip pages
    while (lpddsprimary->Flip(NULL, DDFLIP_WAIT) != DD_OK)
      ;

  } // if ( update )

  return 1; // return success
} // end Game_Main
