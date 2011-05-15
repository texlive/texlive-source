/*

Simple tray menu

Originally written in 2011 by Tomasz M. Trzeciak, Public Domain

compiling with gcc (size optimized):
echo 1 ICON "tl-tray-menu.ico">tl-tray-menu.rc
windres tl-tray-menu.rc tl-tray-menu-rc.o
gcc -Os -s -mwindows -o tl-tray-menu.exe tl-tray-menu-rc.o tl-tray-menu.c

*/

#define _WIN32_IE 0X0500 // minimum support for tray baloon (IE5)

#include <windows.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_CMD 32768

static char msgbuf[4*MAX_PATH];
#define DIE(...) { \
  _snprintf( msgbuf, 4*MAX_PATH, __VA_ARGS__ ); \
  MessageBox( NULL, msgbuf, "ERROR!", MB_ICONERROR | MB_SETFOREGROUND );\
  return 1; \
}

#define IDI_ICON       1    // resource number of icon 
#define IDI_TRAY       100
#define WM_TRAYMSG     101
#define MENU_TLMGR     0
#define MENU_TEXDOC    1
#define MENU_EDITOR    2
#define MENU_COMSPEC   3
#define MENU_CUSTOM    4
#define MENU_EXIT      42

HMENU hPopMenu;
NOTIFYICONDATA nid;

char szAppName[] = "TrayMenu";
char ExeList[5][MAX_PATH] = {
  "tlmgr-gui.exe",
  "texdoctk.exe",
  "texworks.exe",
  "cmd.exe",
  ""
};

int CustomHowMessageBox()
{
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
  switch ( msg )             
  {
  case WM_CREATE:
  {
    hPopMenu = CreatePopupMenu();
    if ( ExeList[MENU_TLMGR][0] )
      AppendMenu( hPopMenu, MF_STRING, MENU_TLMGR, "&Package Manager (tlmgr)" );
    if ( ExeList[MENU_TEXDOC][0] )
      AppendMenu( hPopMenu, MF_STRING, MENU_TEXDOC, "&Documentation (texdoc)" );
    if ( ExeList[MENU_EDITOR][0] )
      AppendMenu( hPopMenu, MF_STRING, MENU_EDITOR, "&Editor (texworks)" );
    if ( ExeList[MENU_COMSPEC][0] )
      AppendMenu( hPopMenu, MF_STRING, MENU_COMSPEC, "&Command Prompt" );
    if ( ExeList[MENU_CUSTOM][0] )
      AppendMenu( hPopMenu, MF_STRING, MENU_CUSTOM, "Custom &Script" );
    else
    AppendMenu( hPopMenu, MF_SEPARATOR, 0, NULL );
    AppendMenu( hPopMenu, MF_STRING, MENU_EXIT,    "E&xit" );
  }
  break;
  
  case WM_TRAYMSG:
  {
    switch ( lParam )
    {
      case WM_LBUTTONUP:
      case WM_RBUTTONUP:
      {
        POINT pnt;
        GetCursorPos( &pnt );
        SetForegroundWindow( hWnd ); // needed to get keyboard focus
        TrackPopupMenu( hPopMenu, TPM_LEFTALIGN, pnt.x, pnt.y, TPM_LEFTBUTTON, hWnd, NULL );
      }
      break;
    }
  }
  break;
  
  case WM_COMMAND:
    switch ( LOWORD( wParam ) )
    {
      case MENU_TLMGR:
      case MENU_TEXDOC:
      case MENU_EDITOR:
      case MENU_COMSPEC:
        if ( _spawnlp( _P_NOWAIT, ExeList[LOWORD(wParam)], NULL ) < 0 )
          DIE( "Failed to start: %s", ExeList[LOWORD(wParam)] );
      break;
      case MENU_CUSTOM:
        if ( GetFileAttributes( ExeList[LOWORD(wParam)] ) != INVALID_FILE_ATTRIBUTES )
          {
            if ( _spawnlp( _P_NOWAIT, ExeList[LOWORD(wParam)], NULL ) < 0 )
              DIE( "Failed to start: %s", ExeList[LOWORD(wParam)] );
          }
        else
          MessageBox(
             NULL,
             "For a custom menu, create tl-tray-menu-custom.bat \nin the root of the installation.\nSee sample at tlpkg\\installer\\tl-tray-menu-custom.bat.",
             "Custom menu",
             MB_ICONINFORMATION
          );
          break;

      case MENU_EXIT:
        Shell_NotifyIcon( NIM_DELETE, &nid );
        ExitProcess(0);
      break;
    }
  break;

  default:                 
    return DefWindowProc( hWnd, msg, wParam, lParam );
  }

  return 0;
}

int APIENTRY WinMain(
				HINSTANCE hInstance,
				HINSTANCE hPrevInstance,
				LPSTR     lpCmdLine,
				int       nCmdShow )
{

  // get file name of this executable
  
  static char selfdir[MAX_PATH];
  char *s;
  DWORD nchars = GetModuleFileName(NULL, selfdir, MAX_PATH);
  if ( !nchars || ( nchars == MAX_PATH ) ) DIE( "cannot get own path" );
  if ( s = strrchr( selfdir, '\\' ) ) *s = '\0'; // remove file name part
  
  // append bin/win32 to PATH
  
  static char buffer[MAX_CMD];
  strcpy( buffer, selfdir );
  strcat( buffer, "\\bin\\win32;" );
  strcat( buffer, getenv( "PATH" ) );
  SetEnvironmentVariable( "PATH", buffer );
  
  // set up a list of executables
  
  s = strchr( buffer, ';' );  *s = '\\';  s++; 
  int i;
  for ( i = 0; i < 3; i++ ) 
  {
    *s = '\0'; 
    strcat( buffer, ExeList[i] );
    if ( GetFileAttributes( buffer ) != INVALID_FILE_ATTRIBUTES )
      strncpy( ExeList[i], buffer, MAX_PATH );
    else
      ExeList[i][0] = '\0';
  }
  
  if ( s = getenv( "COMSPEC" ) ) strncpy( ExeList[3], s, MAX_PATH );
  
  strcpy( buffer, selfdir );
  strcat( buffer, "\\tl-tray-menu-custom.bat" );
  // if ( GetFileAttributes( buffer ) != INVALID_FILE_ATTRIBUTES )
    // At call time, we test for this file and display
    // a message box if it is missing.
    strncpy( ExeList[4], buffer, MAX_PATH );
    
	// create a hidden window for messaging

	MSG msg;
	WNDCLASS wc;
	HWND hWnd;

	ZeroMemory( &wc, sizeof wc );
	wc.hInstance     = hInstance;
	wc.lpszClassName = szAppName;
	wc.lpfnWndProc   = (WNDPROC)WndProc;
	wc.style         = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hIcon         = LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor       = LoadCursor( NULL, IDC_ARROW );

	if ( FALSE == RegisterClass( &wc ) ) DIE( "Failed to register window class" );

	hWnd = CreateWindow(
		szAppName,
		"",
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		hInstance,
		0);

	if ( hWnd == NULL ) DIE( "Failed to create window" );

	// tray icon stuff
  
	nid.cbSize = sizeof nid;
	nid.hWnd = hWnd;
	nid.uID = IDI_TRAY;
	nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
	nid.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( IDI_ICON ) ); 
	nid.uCallbackMessage = WM_TRAYMSG;
	strcpy( nid.szTip, "TeX Live Menu" );
#if (_WIN32_IE >= 0X0500)
  nid.uFlags |= NIF_INFO;
  nid.dwInfoFlags = NIIF_INFO;
  strcpy( nid.szInfo, "Click on the tray icon\nto activate the menu." );
  strcpy( nid.szInfoTitle, nid.szTip );
#endif
	Shell_NotifyIcon( NIM_ADD, &nid );
  
	// main message loop:
  
	while ( GetMessage( &msg, NULL, 0, 0 ) > 0 )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return msg.wParam;
}

