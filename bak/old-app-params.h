#ifndef APP_PARAMS_H
#define APP_PARAMS_H

#include "rect.h"

struct AppParams {
  // Win32 Specific Globals /////////////////////////////////
  HWND main_window_handle; // save the window handle
  HINSTANCE main_instance; // save the instance

  // Direct Draw Specific Globals ///////////////////////////
  LPDIRECTDRAW lpdd;                // dd object
  LPDIRECTDRAWSURFACE lpddsprimary; // dd primary surface
  LPDIRECTDRAWSURFACE lpddsback;    // dd back surface
  LPDIRECTDRAWPALETTE lpddpal;      // a pointer to the created dd palette
  DDSURFACEDESC ddsd;               // a direct draw surface descriptor struct
  DDSCAPS ddscaps;                  // a direct draw surface capabilities struct

  HRESULT ddrval; // result back from dd calls

  UCHAR* double_buffer;      // the double buffer
  PALETTEENTRY palette[256]; // the palette
};

#endif
