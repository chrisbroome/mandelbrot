// includes ///////////////////////////////////////////////
#include <iostream>		// C++ stuff
#include <fstream>		//
#include <iomanip>		//
#include <cstring>		//
#include <cmath>		// sin
#include <ctime>

#include "types.h"		// user defined types and expressions
#include "utilities.h"	// important utilities

using namespace std;

// CONSTANTS //////////////////////////////////////////////

// enumerated itarted function types
enum ITERATED_FUNCTION
{
	mandlebrot = 1,
	julia,
	newton1
};

// constants for windows
const	CHAR	* WINDOW_CLASS_NAME =	"WINXCLASS";		// class name
const	UINT	SCREEN_BPP			=	8;				// number of bits per pixel (2 to this power gives the number of colors)

// PROTOTYPES /////////////////////////////////////////////

int		Game_Init		( void * parms = NULL );
int		Game_Shutdown	( void * parms = NULL );
int		Game_Main		( void * parms = NULL );

void	Welcome		( LPDIRECTDRAWSURFACE lpdds );

BYTE	Mandlebrot	( const LDOUBLE & a, const LDOUBLE & b, const BYTE & numiterations );
BYTE	Julia		( const LDOUBLE & xparam, const LDOUBLE & yparam, const LDOUBLE & a, const LDOUBLE & b, const BYTE & numiterations );

BOOL	LoadValues	( const char * filename, LDOUBLE & amin, LDOUBLE & bmin, LDOUBLE & amax, LDOUBLE & bmax );
BOOL	LoadValues	( const char * filename, Rect<LDOUBLE> & rect );
void	SaveValues	( const char * filename, LDOUBLE amin, LDOUBLE bmin, LDOUBLE amax, LDOUBLE bmax );
void	SaveValues	( const char * filename, Rect<LDOUBLE> rect );

void	Gradient	( COLORREF c1, COLORREF c2, UINT start, UINT finish );
void	Gradient	( PALETTEENTRY c1, PALETTEENTRY c2, UINT start, UINT finish );
void	Gradient	( PALETTEENTRY colorarray[], WORD numcolors, UINT start, UINT finish );

// GLOBALS ////////////////////////////////////////////////

// do not change after initialization!!!
Rect<UINT> Window( Point<UINT>(0,0), Point<UINT>(640,480) );
Rect<UINT> Screen( Point<UINT>(0,0), Point<UINT>(640,480) );

// change all you want after initialization
const char *		default_filename = "mandvals.txt";

Rect<UINT>		View	( Window );

Rect<LDOUBLE>	World	( Point<LDOUBLE>(-2, 2), Point<LDOUBLE>(-2, 2) );

// Win32 Specific Globals /////////////////////////////////
HWND				main_window_handle	= NULL;	// save the window handle
HINSTANCE			main_instance		= NULL;	// save the instance

// Direct Draw Specific Globals ///////////////////////////
LPDIRECTDRAW		lpdd				= NULL;	// dd object
LPDIRECTDRAWSURFACE	lpddsprimary		= NULL,	// dd primary surface
					lpddsback			= NULL;	// dd back surface
LPDIRECTDRAWPALETTE	lpddpal				= NULL;	// a pointer to the created dd palette
DDSURFACEDESC		ddsd;						// a direct draw surface descriptor struct
DDSCAPS				ddscaps;					// a direct draw surface capabilities struct
HRESULT				ddrval;						// result back from dd calls

UCHAR				*double_buffer		= NULL;	// the double buffer              
PALETTEENTRY		palette[256]		= {0};


// Functions //////////////////////////////////////////////
LRESULT CALLBACK WindowProc ( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	// this is the main message handler of the system
	PAINTSTRUCT	ps;		   // used in WM_PAINT
	HDC			hdc;	   // handle to a device context

	// what is the message 
	switch ( msg )
	{	
		case WM_CREATE:
		{
			// do initialization stuff here
			return(0);
		} break;

		case WM_PAINT:
		{
			// start painting
			hdc = BeginPaint(hwnd,&ps);

			// end painting
			EndPaint(hwnd,&ps);
			return(0);
		} break;

		case WM_DESTROY:
		{
			// kill the application			
			PostQuitMessage(0);
			return(0);
		} break;

		default:
			break;

    } // end switch

	// process any messages that we didn't take care of 
	return ( DefWindowProc( hwnd, msg, wparam, lparam ) );

} // end WinProc

// WINMAIN ////////////////////////////////////////////////

int WINAPI WinMain ( HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow )
{

	bool		done = false;	// used in the while loop.  set to true if finished with program
	WNDCLASS	winclass;		// this will hold the class we create
	HWND		hwnd;			// generic window handle
	MSG			msg;			// generic message
//	HMENU		hMenu;			// generic menu
//	HDC			hdc;			// generic dc
//	PAINTSTRUCT	ps;				// generic paintstruct


	// first fill in the window class stucture
	winclass.style			= CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= WindowProc;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon ( NULL, IDI_APPLICATION );
	winclass.hCursor		= LoadCursor ( NULL, IDC_CROSS );
	winclass.hbrBackground	= NULL;		// GetStockObject ( BLACK_BRUSH );
	winclass.lpszMenuName	= NULL;		// MAKEINTRESOURCE ( IDR_MENU1 );
	winclass.lpszClassName	= WINDOW_CLASS_NAME;

	// register the window class
	if (!RegisterClass(&winclass))
		return(0);

	// create the window, note the use of WS_POPUP
	if (!( hwnd = CreateWindow ( WINDOW_CLASS_NAME,		// class
							"WinX Game Console",		// title
							WS_POPUP | WS_VISIBLE | WS_EX_TOPMOST ,		// properties of the window
							0,0,						// x,y
							Window.Width(), Window.Height(), // width, height
							NULL,		// handle to parent
							NULL,		// handle to menu
							hinstance,	// instance
							NULL)))		// creation parms
		return(0);

	// save the window handle and instance in a global
	main_window_handle = hwnd;
	main_instance      = hinstance;

	//	perform all game console specific initialization
	Game_Init();

	// enter main event loop
	while ( !done )
	{
		if ( PeekMessage ( &msg, NULL, 0, 0, PM_REMOVE ) )
		{ 
			// test if this is a quit
			if ( msg.message == WM_QUIT )
				done = true;
	
			// translate any accelerator keys
			TranslateMessage ( &msg );

			// send the message to the window proc
			DispatchMessage ( &msg );
		} // end if
    
		// main game processing goes here
		Game_Main();

	} // end while

	// shutdown game and release all resources
	Game_Shutdown();

	// return to Windows like this
	return msg.wParam;
} // end WinMain

// WINX GAME PROGRAMMING CONSOLE FUNCTIONS ////////////////

int Game_Init ( void *parms )
{
	// this function is where you do all the initialization for your game

	// create object and test for error
	if ( DirectDrawCreate ( NULL, &lpdd, NULL ) != DD_OK )
		return ( 0 );

	// set cooperation level to windowed mode normal
	if ( lpdd->SetCooperativeLevel ( main_window_handle,
		DDSCL_EXCLUSIVE
		| DDSCL_FULLSCREEN
		| DDSCL_ALLOWMODEX
		| DDSCL_ALLOWREBOOT
		) != DD_OK )
	return 0;

	// set the display mode
	if ( lpdd->SetDisplayMode ( Screen.Width(), Screen.Height(), SCREEN_BPP ) != DD_OK )
		return 0;

	// Create the primary surface
	memset ( &ddsd, 0, sizeof ( ddsd ) );
	ddsd.dwSize = sizeof ( ddsd );

	// set the flags to validate both the capabilities 
	// field and the backbuffer count field
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;

	// we need to let dd know that we want a complex 
	// flippable surface structure, set flags for that
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;

	// set the backbuffer count to 1
	ddsd.dwBackBufferCount = 1;

	// create the primary surface
	lpdd->CreateSurface ( &ddsd, &lpddsprimary, NULL );

	// query for the backbuffer or secondary surface
	// notice the use of ddscaps to indicate what 
	// we are requesting
	ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

	// get the surface
	lpddsprimary->GetAttachedSurface ( &ddscaps, &lpddsback );

	// allocate memory for the double buffer
	double_buffer = new UCHAR [Screen.Width() * Screen.Height()];
	if ( !double_buffer )
		return(0);

	/*
		Red		ff	0	0
		Orange	ff	7f	0
		Yellow	ff	ff	0
		Y - G	7f	ff	0
		Green	0	ff	0
		Blue	0	0	ff
		Purple	ff	0	ff
		Pink	7f	0	7f
		Black	0	0	0
	*/
	PALETTEENTRY grad[9] =
	{
		{ 0xff, 0x00, 0x00, PC_NOCOLLAPSE },
		{ 0xff, 0x7f, 0x00, PC_NOCOLLAPSE },
		{ 0xff, 0xff, 0x00, PC_NOCOLLAPSE },
		{ 0x7f, 0xff, 0x00, PC_NOCOLLAPSE },
		{ 0x00, 0xff, 0x00, PC_NOCOLLAPSE },
		{ 0x00, 0x00, 0xff, PC_NOCOLLAPSE },
		{ 0xff, 0x00, 0xff, PC_NOCOLLAPSE },
		{ 0x7f, 0x00, 0x7f, PC_NOCOLLAPSE },
		{ 0x00, 0x00, 0x00, PC_NOCOLLAPSE }
	};

	Gradient ( grad, 9, 0, 255 );
//	LoadValues ( default_filename, World.p1.x, World.p1.y, World.p2.x, World.p2.y );
	Welcome ( lpddsprimary );

	// return success
	return 1;

} // end Game_Init

///////////////////////////////////////////////////////////

int Game_Shutdown(void *parms)
{
	// this function is where you shutdown your game and
	// release all resources that you allocated

	// first release the secondary surface
	if ( lpddsback )
		lpddsback->Release();
       
	// now release the primary surface
	if ( lpddsprimary )
		lpddsprimary->Release();
       
	// release the directdraw object
	if ( lpdd )
		lpdd->Release();

	// free double buffer
	if ( double_buffer )
		delete [] double_buffer;

	// return success
	return 1;
} // end Game_Shutdown

///////////////////////////////////////////////////////////

int Game_Main ( void *parms )
{
	// this is the workhorse of your game it will be called continuously in real-time this is like main() in C
	// all the calls for you game go here!

	UCHAR	*	back_buffer = NULL, // used to draw
			*	dest_ptr    = NULL, // used in line by line copy
			*	src_ptr     = NULL; // " "

	static ITERATED_FUNCTION whichfunction	=	mandlebrot;

	static BYTE				numits			=	254;
	static BOOL				update			=	false;
	static BOOL				done			=	false;
	static BOOL				display_stats	=	false;

	static Point<UINT>		zoomspeed( 80, 60 );
	static Point<LDOUBLE>	inc;
	static Point<LDOUBLE>	orbitPoint;

	static char				s[200];

	static Point<LDOUBLE>	jparam ( 0, 0 );

	if ( KEY_DOWN ( VK_C ) )
	{
		POINT	p1;
		HDC		hdc;
		bool	abortChooseC = false;
		lpddsprimary->GetDC ( &hdc );
		while ( !KEY_DOWN ( VK_LBUTTON ) && !abortChooseC )
		{
			GetCursorPos ( & p1 );
			orbitPoint.x =	p1.x * ( World.Width() / View.Width () )
					+ ( ( World.Center().x - View.Center().x ) * World.Width() ) / View.Width()
					+ World.Center().x;
			orbitPoint.y =	-(	p1.y * ( World.Height() / View.Height () )
					+ ( ( World.Center().y - View.Center().y ) * World.Height() ) / View.Height()
					)
					+ World.Center().y;
			sprintf ( s, "x0 = %16.14lf + %16.14lfi", orbitPoint.x, orbitPoint.y );
			TextOut ( hdc, 0, 16*1, s, strlen(s) );
			if ( KEY_DOWN ( VK_ESCAPE ) )
			{
				abortChooseC = true;
				while ( KEY_DOWN ( VK_ESCAPE ) )
					;
			}
		}
		lpddsprimary->ReleaseDC ( hdc );
	}
	if ( KEY_DOWN ( VK_Z ) )
	{
		while ( KEY_DOWN ( VK_Z ) )
			;

		POINT	p1, p2;
		HDC		hdc;
		bool	abortZoom = false;
		lpddsprimary->GetDC ( &hdc );
		Rect <LDOUBLE> zoom;

		sprintf ( s, "Zooming Function" );
		TextOut ( hdc, 0, 0, s, strlen(s) );

		while ( !KEY_DOWN ( VK_LBUTTON ) && !abortZoom )
		{
			GetCursorPos ( & p1 );
			zoom.p1.x =		p1.x * ( World.Width() / View.Width () )
							+ ( ( World.Center().x - View.Center().x ) * World.Width() ) / View.Width()
							+ World.Center().x;
			zoom.p1.y =	-(	p1.y * ( World.Height() / View.Height () )
							+ ( ( World.Center().y - View.Center().y ) * World.Height() ) / View.Height()
							)
							+ World.Center().y;
			sprintf ( s, "Mouse Position: (%4d,%4d) c = %16.14lf + %16.14lfi", p1.x, p1.y, zoom.p1.x, zoom.p1.y );
			TextOut ( hdc, 0, 16*1, s, strlen(s) );
			if ( KEY_DOWN ( VK_ESCAPE ) )
				abortZoom = true;
		}
//		RECT r;
		while ( KEY_DOWN ( VK_LBUTTON ) && !abortZoom )
		{
			GetCursorPos ( &p2 );
/*
			r.top = p1.x;
			r.left = p1.y;
			r.bottom = p2.x;
			r.right = p2.y;
*/
			zoom.p2.x =		p2.x * ( World.Width() / View.Width () )
							+ ( ( World.Center().x - View.Center().x ) * World.Width() ) / View.Width()
							+ World.Center().x;
			zoom.p2.y =	-(	p2.y * ( World.Height() / View.Height () )
							+ ( ( World.Center().y - View.Center().y ) * World.Height() ) / View.Height()
							)
							+ World.Center().y;
			
			RECT r;

			r.left = p1.x;
			r.top = p1.y;
			r.right = p2.x;
			r.bottom = p2.y;

//			InvertRect( hdc, &r );
			sprintf ( s, "Mouse Position: (%4d,%4d) c = %16.14lf + %16.14lfi", p2.x, p2.y, zoom.p2.x, zoom.p2.y );
			TextOut ( hdc, 0, 16*1, s, strlen(s) );
			if ( KEY_DOWN ( VK_ESCAPE ) )
				abortZoom = true;
		}

		if ( !abortZoom )
		{
			zoom.Normalize();
//			View.Normalize();
			World = zoom;
			sprintf(s, "New coordinates: [(%16.14lf,%16.14lf),(%16.14lf,%16.14lf)]",
				World.p1.x,
				World.p1.y,
				World.p2.x,
				World.p2.y
				);
			TextOut ( hdc, 0, 16*2, s, strlen(s) );
//			MessageBox ( main_window_handle, s, "New coordinates", 0 );
			update = true;
		}
		else
		{
			while ( KEY_DOWN ( VK_ESCAPE ) )
				;
		}
		lpddsprimary->ReleaseDC ( hdc );
	}
/*
	if ( KEY_DOWN ( VK_N ) )
	{
		whichfunction = newton1;
		update = true;
	}
*/
	if ( KEY_DOWN ( VK_M ) )
	{
		whichfunction = mandlebrot;
		update	= true;
	}
	if ( KEY_DOWN ( VK_J ) )
	{
		while ( KEY_DOWN ( VK_J ) )
			;
		POINT	p1;
		HDC		hdc;
		lpddsprimary->GetDC ( &hdc  );

		sprintf ( s, "Julia Set for:");
		TextOut ( hdc, 0, 0, s, strlen(s) );

		while ( !KEY_DOWN ( VK_LBUTTON ) )
		{
			GetCursorPos ( & p1 );
			jparam.x =		p1.x * ( World.Width() / View.Width () )
							+ ( ( World.Center().x - View.Center().x ) * World.Width() ) / View.Width()
							+ World.Center().x;
			jparam.y =	-(	p1.y * ( World.Height() / View.Height () )
							+ ( ( World.Center().y - View.Center().y ) * World.Height() ) / View.Height()
							)
							+ World.Center().y;
			sprintf ( s, "c = %16.14lf + %16.14lfi", jparam.x, jparam.y );
			TextOut ( hdc, 0, 16*1, s, strlen(s) );
		}
		while ( KEY_DOWN ( VK_LBUTTON ) )
		{
			GetCursorPos ( & p1 );
			jparam.x =		p1.x * ( World.Width() / View.Width () )
							+ ( ( World.Center().x - View.Center().x ) * World.Width() ) / View.Width()
							+ World.Center().x;
			jparam.y =	-(	p1.y * ( World.Height() / View.Height () )
							+ ( ( World.Center().y - View.Center().y ) * World.Height() ) / View.Height()
							)
							+ World.Center().y;
			sprintf ( s, "c = %16.14lf + %16.14lfi", jparam.x, jparam.y );
			TextOut ( hdc, 0, 16*1, s, strlen(s) );
		}

		lpddsprimary->ReleaseDC ( hdc );

		display_stats = true;
		update	= true;
		whichfunction = julia;
	}
	if ( KEY_DOWN ( VK_O ) )
	{
		while ( KEY_DOWN ( VK_O ) || KEY_DOWN ( VK_LBUTTON ) )
			;
		Point<ULONG>		p1 ( Screen.HalfWidth(), Screen.HalfHeight() );
		HDC		hdc;
		lpddsprimary->GetDC ( &hdc );
		sprintf ( s, "Orbit Analysis for the point %16.14lfi + %16.14lfi: ", orbitPoint.x, orbitPoint.y );
		TextOut ( hdc, 0, 0, s, strlen(s) );

		SelectObject ( hdc, CreatePen ( PS_SOLID, 0, RGB ( 64+rand()%(256-64),64+rand()%(256-64),64+rand()%(256-64) ) ) );

		LDOUBLE	x		= 0,
				y		= 0,
				a		= orbitPoint.x,
				b		= orbitPoint.y,
				xtemp,
				ytemp;

		WORD	count	= 0, numiterations = 256;

		MoveToEx ( hdc, p1.x, p1.y, NULL );
		while ( (x > -2 && x < 2 && y > -2 && y < 2) && count < numiterations )
		{
			p1.x = (View.Width()  )+(+x*View.Width() +World.Width() *World.Center().x-World.Width() *View.Center().x+World.Center().x*View.Width()) /World.Width();
			p1.y = (View.Height() )+(-y*View.Height()+World.Height()*World.Center().y-World.Height()*View.Center().y+World.Center().y*View.Height())/World.Height();
			LineTo ( hdc, p1.x, p1.y );
			sprintf ( s, "x%-3d = %16.14lf + %16.14lfi", count, x, y, p1.x, p1.y );
			TextOut ( hdc, 0, 16*2, s, strlen(s) );

			xtemp = x;
			ytemp = y;

			x = xtemp*xtemp - ytemp*ytemp + a;	// x^2 - y^2 + a
			y = 2*xtemp*ytemp + b;				// 2xy + b

			++count;
		}
		lpddsprimary->ReleaseDC ( hdc );
	}
	if ( KEY_DOWN ( VK_RETURN ) )
	{
		update	= true;
	}
	if ( KEY_DOWN ( VK_ESCAPE ) )
	{
		done = true;
		PostQuitMessage ( 0 );
	}
	if ( KEY_DOWN ( VK_HOME ) )
	{
		++numits;
		update = true;
	}
	if ( KEY_DOWN ( VK_END ) )
	{
		--numits;
		update = true;
	}
	if ( KEY_DOWN ( VK_DOWN ) )
	{
		World.p1.y += inc.y*zoomspeed.y;
		World.p2.y += inc.y*zoomspeed.y;
		update = true;
	}
	if ( KEY_DOWN ( VK_UP ) )
	{
		World.p1.y -= inc.y*zoomspeed.y;
		World.p2.y -= inc.y*zoomspeed.y;
		update = true;
	}
	if ( KEY_DOWN ( VK_RIGHT ) )
	{
		World.p1.x -= inc.x*zoomspeed.x;
		World.p2.x -= inc.x*zoomspeed.x;
		update = true;
	}
	if ( KEY_DOWN ( VK_LEFT ) )
	{
		World.p1.x += inc.x*zoomspeed.x;
		World.p2.x += inc.x*zoomspeed.x;
		update = true;
	}
	if ( KEY_DOWN ( VK_PRIOR ) )			// zoom in
	{
		World.p1.x	+=	inc.x * zoomspeed.x;
		World.p2.x	-=	inc.x * zoomspeed.x;
		World.p1.y	+=	inc.y * zoomspeed.y;
		World.p2.y	-=	inc.y * zoomspeed.y;
		update	=	true;
	}
	if ( KEY_DOWN ( VK_NEXT ) )		// zoom out
	{
		World.p1.x	-=	inc.x * zoomspeed.x;
		World.p2.x	+=	inc.x * zoomspeed.x;
		World.p1.y	-=	inc.y * zoomspeed.y;
		World.p2.y	+=	inc.y * zoomspeed.y;
		update	=	true;
	}
	if ( KEY_DOWN ( VK_P ) )
	{
		while ( KEY_DOWN ( VK_P ) )
			;

		HDC hdc;
		lpddsprimary->GetDC ( & hdc );
		sprintf(s, "Bitmap saving not implemented yet" );
		TextOut ( hdc, 0, 16, s, strlen(s) );
		lpddsprimary->ReleaseDC ( hdc );
	}
	if ( KEY_DOWN ( VK_L ) )		// load mandlebrot zoom rectangle values
	{
		while ( KEY_DOWN ( VK_S ) )
			;
		LoadValues ( default_filename, World );
		World.Normalize();
		update  = true;
	}
	if ( KEY_DOWN ( VK_S ) )		// save mandlebrot zoom rectangle values
	{
		while ( KEY_DOWN ( VK_S ) )
			;
		SaveValues ( default_filename, World );
	}
	if ( KEY_DOWN ( VK_SPACE ) )	// reset to default view
	{
		World.p1.x = World.p1.y = -2.0;
		World.p2.x = World.p2.y = 2.0;
		update = true;
	}
	if ( KEY_DOWN ( VK_LSHIFT ) )
	{
		display_stats ? display_stats = false : display_stats = true;
		update = true;
		while ( KEY_DOWN ( VK_LSHIFT ) ) { }
	}
	if ( KEY_DOWN ( VK_SUBTRACT ) )
	{
		zoomspeed.x -= 4;
		zoomspeed.y -= 3;
		while ( KEY_DOWN ( VK_SUBTRACT ) ) { }
	}
	if ( KEY_DOWN ( VK_ADD ) )
	{
		zoomspeed.x += 4;
		zoomspeed.y += 3;
		while ( KEY_DOWN ( VK_ADD ) ) { }
	}
	if ( update )
	{
		// erase secondary back buffer
		memset ( &ddsd, 0, sizeof(ddsd) );
		ddsd.dwSize = sizeof(ddsd);

		// lock the secondary surface
		lpddsback->Lock ( NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL );

		// get video pointer to secondary surface
		back_buffer = (UCHAR *)ddsd.lpSurface;

		// clear out the back_buffer
		if ( ddsd.lPitch == Screen.Width() )
			memset ( back_buffer, 0, Screen.Width() * Screen.Height() );
		// non-linear memory
		else
		{
			// make copy of video pointer
			dest_ptr = back_buffer;

			// clear out memory one line at a time
			for ( int y = 0; y < Screen.Height(); ++y )
			{
				// clear next line
				memset ( dest_ptr, 0, Screen.Width() );
				// advance pointer to next line
				dest_ptr += ddsd.lPitch;
			} // end for y

		} // end else

		// perform game logic...
       
		// draw the next frame into the secondary back buffer

		HDC	hdc;
		lpddsprimary->GetDC( &hdc );
		sprintf ( s, "Updating...");
		TextOut ( hdc, 0, View.Height()-16, s, strlen(s) );
		lpddsprimary->ReleaseDC( hdc );

		LDOUBLE a;
		LDOUBLE b = World.p1.y;

		time_t last;
		time_t now;

		time(&now);
		last = now - 1;

		inc.x = (World.p2.x - World.p1.x) / View.Width();
		inc.y = (World.p2.y - World.p1.y) / View.Height();

		for ( int y = View.Height() - 1; y >= 0; --y )
		{
			a = World.p1.x;
			for ( int x = 0; x < View.Width(); ++x )
			{
				switch ( whichfunction )
				{
					case julia:
						back_buffer [ x + y * ddsd.lPitch ] =	Julia	( a, b, jparam.x, jparam.y, numits );
						break;
					case mandlebrot:
						back_buffer [ x + y * ddsd.lPitch ] =	Mandlebrot	( a, b, numits );
						break;
//					case newton1:
//						back_buffer [ x + y * ddsd.lPitch ] =	Newton1	( a, b, numits );
						break;
					default:
						back_buffer [ x + y * ddsd.lPitch ] =	0;
						break;
				}
				a += inc.x;
				if ( last < now )
				{
					lpddsprimary->GetDC( &hdc );
					sprintf ( s, "Updating: %8.5lf%%", 100*LDOUBLE(x+(View.Height()-y)*View.Width())/(View.Width()*View.Height()) );
					TextOut ( hdc, 0, View.Height()-16, s, strlen(s) );
					lpddsprimary->ReleaseDC( hdc );
					last = now;
				}
				time(&now);
			}
			b += inc.y;
		}
		update = false;
		// linear memory

		// unlock secondary buffer
		lpddsback->Unlock ( back_buffer );

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
		while ( lpddsprimary->Flip ( NULL, DDFLIP_WAIT ) != DD_OK );

	} // if ( update )

	return 1;	// return success
} // end Game_Main

inline ULONG FastFact( const ULONG n )
{
	switch (n)
	{
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

inline ULONG NchooseK( const ULONG n, const ULONG k )
{
	return (FastFact(n)/FastFact(k))/FastFact(n-k);
}


LDOUBLE QuickPow(LDOUBLE base, ULONG exponent)
{
	LDOUBLE a = 1.0;
	LDOUBLE b = base;
	ULONG e = exponent;
	// inv 1:base ˆ exponent = a * b ˆ e and e >= 0
	while( e > 0 )
	{
		// inv 2: base ˆ exponent = a * b ˆ e and e >= 0
		if( e % 2 != 0 )
			a = a * b;
		b = b * b;
		e = e / 2;
		// inv 3: base ˆ exponent = a * b ˆ e and e >= 0
	}// inv 4: base ˆ exponent = a and e == 0
	return a;
}

VOID ComplexBinomial( const ULONG power, const LDOUBLE x, const LDOUBLE y, LDOUBLE & realAns, LDOUBLE &imagAns )
{
	const ULONG n = power;
	const ULONG nOver2 = n / 2;

	ULONG twoK, twoKPlus1;
	LONG minus1 = -1;

	LDOUBLE xsum = 0,
			ysum = 0;

	for ( ULONG k=0; k <= nOver2; ++k )
	{
		twoK = 2*k;
		twoKPlus1 = twoK + 1;
		minus1 = -1*minus1;
		ysum += minus1*NchooseK(n,twoK)*pow(x,twoK)*pow(y,n-twoK);
		if ( twoKPlus1 <= n )
			xsum += minus1*NchooseK(n,twoKPlus1)*pow(x,twoKPlus1)*pow(y,n-twoKPlus1);
	}
	realAns = xsum;
	imagAns = ysum;
}

BYTE Mandlebrot ( const LDOUBLE & a, const LDOUBLE & b, const BYTE & numiterations )
{
	LDOUBLE	x		= 0,
    		y		= 0,
			xtemp,
			ytemp;

	WORD	count	= 0;

	if ( a < -2 || a > 2 || b < -2 || b > 2 ) return 0;
	for ( ; count < numiterations; ++count )
	{
		xtemp = x;
		ytemp = y;

		if ( x*x + y*y > 4 )
			return count;

//		x = xtemp*xtemp - ytemp*ytemp + a;	// x^2 - y^2 + a
//		y = 2*xtemp*ytemp + b;				// 2xy + b

//		x = 5*xtemp*ytemp*ytemp*ytemp*ytemp - 10*xtemp*xtemp*xtemp*ytemp*ytemp + xtemp*xtemp*xtemp*xtemp*xtemp + a;
//		y = ytemp*ytemp*ytemp*ytemp*ytemp - 10*xtemp*xtemp*ytemp*ytemp*ytemp + 5*xtemp*xtemp*xtemp*xtemp*ytemp + b;

//		x = 5*xtemp*QuickPow(ytemp,4) - 10*QuickPow(xtemp,3)*QuickPow(ytemp,2) + QuickPow(xtemp,5) + a;
//		y = QuickPow(ytemp,5) - 10*QuickPow(xtemp,2)*QuickPow(ytemp,3) + 5*QuickPow(xtemp,4)*ytemp + b;
//		ComplexBinomial( 5, xtemp, ytemp, x, y );
//		x += a;
//		y += b;

		x = ytemp*ytemp*ytemp*ytemp*ytemp*ytemp -
			15*xtemp*xtemp*ytemp*ytemp*ytemp*ytemp +
			15*ytemp*ytemp*xtemp*xtemp*xtemp*xtemp -
			xtemp*xtemp*xtemp*xtemp*xtemp*xtemp +
			a;
		y = 6*xtemp*ytemp*ytemp*ytemp*ytemp*ytemp -
			20*xtemp*xtemp*xtemp*ytemp*ytemp*ytemp +
			6*xtemp*xtemp*xtemp*xtemp*xtemp*ytemp +
			b;
	}
	if ( x*x + y*y < 4 )
		return 0;
	else
		return count;
}

BYTE Julia ( const LDOUBLE & xparam, const LDOUBLE & yparam, const LDOUBLE & a, const LDOUBLE & b, const BYTE & numiterations )
{
	LDOUBLE	x		= xparam,
    		y		= yparam,
			xtemp,
			ytemp;

	WORD	count	= 0;

	if ( xparam < -2 || xparam > 2 || yparam < -2 || yparam > 2 ) return 0;
	for ( ; count < numiterations; ++count )
	{
		xtemp = x;
		ytemp = y;

		if ( x*x + y*y > 4 )
			return count;

		x = xtemp*xtemp - ytemp*ytemp + -a;	// x^2 - y^2 + a
		y = 2*xtemp*ytemp + b;				// 2xy + b
//		x = xtemp*xtemp - ytemp*ytemp + a;	// x^2 - y^2 + a
//		y = 2*xtemp*ytemp + b;				// 2xy + b
	}
	if ( x*x + y*y < 4 )
		return 0;
	else
		return count;
}

void Welcome ( LPDIRECTDRAWSURFACE lpdds )
{
	const UINT MAXLEN	= 85;
	const UINT NUMLINES = 17;
	char message[NUMLINES][MAXLEN] =
    {
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
		"Press SPACEBAR to start"
    };


	HDC hdc;
	lpdds->GetDC ( & hdc );
    for ( UINT i = 0; i < NUMLINES; ++i )
		TextOut ( hdc, 0, i*16, message[i], strlen(message[i]) );

	lpdds->ReleaseDC ( hdc );
}

BOOL LoadValues ( const char * filename, Rect<LDOUBLE> & rect )
{
	ifstream	instr ( filename );
	if ( !instr )
		return false;
	else
	{
		instr >> rect.p1.x >> rect.p1.y >> rect.p2.x >> rect.p2.y;
		return true;
	}
}

BOOL LoadValues ( const char * filename, LDOUBLE & x1, LDOUBLE & y1, LDOUBLE & x2, LDOUBLE & y2 )
{
	ifstream	instr ( filename );
	if ( !instr )
		return false;
	else
	{
		instr >> x1 >> y1 >> x1 >> y2;
		return true;
	}
}

void SaveValues ( const char * filename, LDOUBLE x1, LDOUBLE y1, LDOUBLE x2, LDOUBLE y2 )
{
	FILE * fp;
	fp = fopen ( filename, "wt" );
	fprintf ( fp, "%80.78lf\n%80.78lf\n%80.78lf\n%80.78lf\n", x1, y1, x2, y2 );
	fclose ( fp );
}

void SaveValues ( const char * filename, Rect<LDOUBLE> rect )
{
	FILE * fp;
	fp = fopen ( filename, "wt" );
	fprintf ( fp, "%80.78lf\n%80.78lf\n%80.78lf\n%80.78lf\n", rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y  );
	fclose ( fp );
}

void Gradient ( COLORREF c1, COLORREF c2, UINT start, UINT finish )
{
	WORD	a;
	BYTE	ri = RED ( c1 ),
			gi = GREEN ( c1 ),
			bi = BLUE ( c1 ),
			rf = RED ( c2 ),
			gf = GREEN ( c2 ),
			bf = BLUE ( c2 );

	double	rinc, ginc, binc;
	double	r = 0, g = 0, b = 0;
	BYTE	R, G, B;

	rinc = (double)(rf-ri)/abs(finish - start + 1);
	ginc = (double)(gf-gi)/abs(finish - start + 1);
	binc = (double)(bf-bi)/abs(finish - start + 1);

	R = ri;
	G = gi;
	B = bi;
	for ( a = start; a <= finish; ++a )
	{
		r += rinc;
		g += ginc;
		b += binc;
		R = ri + (BYTE)r;
		G = gi + (BYTE)g;
		B = bi + (BYTE)b;
		palette[a].peRed	= R;
		palette[a].peGreen	= G;
		palette[a].peBlue	= B;
		palette[a].peFlags	= PC_NOCOLLAPSE;
	}
	lpdd->CreatePalette ( DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, palette, &lpddpal, NULL );
	lpddsprimary->SetPalette ( lpddpal );
}

void	Gradient	( PALETTEENTRY c1, PALETTEENTRY c2, UINT start, UINT finish )
{
	WORD			n;
	PALETTEENTRY	i = c1,
					f = c2,
					cur;

	DOUBLE			rinc,
					ginc,
					binc;
	DOUBLE			r = 0,
					g = 0,
					b = 0;

	rinc = (DOUBLE)(f.peRed		- i.peRed)		/abs(finish - start + 1);
	ginc = (DOUBLE)(f.peGreen	- i.peGreen)	/abs(finish - start + 1);
	binc = (DOUBLE)(f.peBlue	- i.peBlue)		/abs(finish - start + 1);

	cur = i;
	for ( n = start; n <= finish; ++n )
	{
		r += rinc;
		g += ginc;
		b += binc;
		cur.peRed	= i.peRed	+ (BYTE)r;
		cur.peGreen	= i.peGreen	+ (BYTE)g;
		cur.peBlue	= i.peBlue	+ (BYTE)b;
		cur.peFlags	= PC_NOCOLLAPSE;
		palette[n]	= cur;
	}
	lpdd->CreatePalette ( DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, palette, &lpddpal, NULL );
//	lpddpal->SetEntries ( 0, start, finish - start + 1, palette );
	lpddsprimary->SetPalette ( lpddpal );
}

void	Gradient	( PALETTEENTRY colorarray[], WORD numelements, UINT start, UINT finish )
{
	UINT	s = start,
			f = finish;

	if ( numelements == 0 )
		return;

	if ( s == f )
	{
		palette[s] = colorarray[numelements/2];
	}
	else
	{
		if ( s > f )
			Swap ( s, f );
		else
		{
			UINT			numcolors = f - s + 1;
			DOUBLE			inc = (DOUBLE)numcolors / (numelements - 1);
			s = start;
			f = s + (WORD)inc - 1;
			COLORREF c1, c2;
			for	( WORD i = 0; i < numelements-1; ++i )
			{
				c1 = RGB ( colorarray[i  ].peRed, colorarray[i  ].peGreen, colorarray[i  ].peBlue );
				c2 = RGB ( colorarray[i+1].peRed, colorarray[i+1].peGreen, colorarray[i+1].peBlue );
//				Gradient ( colorarray[i], colorarray[i+1], s, f );
				Gradient ( c1, c2, s, f );
				s = f + 1;
				f += (WORD)inc;
			}
		}
	}
	lpdd->CreatePalette ( DDPCAPS_8BIT | DDPCAPS_INITIALIZE | DDPCAPS_ALLOW256, palette, &lpddpal, NULL );
	lpddsprimary->SetPalette ( lpddpal );
}
