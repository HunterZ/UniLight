// UniLight by HunterZ
/*
#ifndef _FILE_DEFINED
struct _iobuf {
	char *_ptr;
	int   _cnt;
	char *_base;
	int   _flag;
	int   _file;
	int   _charbuf;
	int   _bufsiz;
	char *_tmpfname;
};
typedef struct _iobuf FILE;
#define _FILE_DEFINED
#endif
*/
#include "ColorUtil.h"
#include "LFXUtil.h"
#include "LLEDUtil.h"
#include "resource.h"

#include <iomanip>
#include <sstream>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>
#include <Wtsapi32.h>
/*
#include <iostream>
#include <io.h>
#include <fcntl.h>
*/
#define ID_TRAY_APP_ICON 1001
#define ID_TRAY_SYNC     1002
#define ID_TRAY_ABOUT    1003
#define ID_TRAY_EXIT     1004
#define WM_SYSICON       (WM_USER + 1)

namespace
{
	UINT WM_TASKBARCREATED(0);
	HMENU Hmenu;
	NOTIFYICONDATA notifyIconData;
	const LPCWSTR szTIP(_T("UniLight"));
	const LPCWSTR szClassName(_T("UniLight"));
	LFXUtil::LFXUtilC lfxUtil;
	LLEDUtil::LLEDUtilC llledUtil;
	COLORREF lastColor(0);
}
/*
// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

void RedirectIOToConsole()
{
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;
}
*/

void ShowAbout(HWND hwnd)
{
	SetForegroundWindow(hwnd);
	static bool active(false);
	if (active) return; // don't allow multiple about popups to be opened
	active = true;
	MessageBox(hwnd, _T("UniLight by HunterZ (https://github.com/HunterZ/UniLight)"), _T("About UniLight"), NULL);
	active = false;
}

void UpdateColor(const COLORREF curColor)
{
	// set AlienFX/LightFX color
	const bool lfxStatus(lfxUtil.SetLFXColor(GetRValue(curColor), GetGValue(curColor), GetBValue(curColor)));

	// set Logitech LED color
	const bool lledStatus(llledUtil.SetLLEDColor(GetRValue(curColor), GetGValue(curColor), GetBValue(curColor)));

	// set tooltip
	std::wstringstream s;
	s << "UniLight status:";
	s << "\nCurrent color: 0x" << std::setfill(L'0') << std::setw(8) << std::hex << curColor;
	s << "\nPrevious color: 0x" << std::setfill(L'0') << std::setw(8) << std::hex << lastColor;
	s << "\nLightFX: " << (lfxStatus ? "active" : "inactive");
	s << "\nLogiLED: " << (lledStatus ? "active" : "inactive");
	StringCchCopy(notifyIconData.szTip, 128, s.str().c_str());
	Shell_NotifyIcon(NIM_MODIFY, &notifyIconData);

	// update last-seen color to new value
	lastColor = curColor;

//	std::cout << std::setfill('0') << std::setw(8) << std::hex << curColor << std::endl;
}

void GetAndUpdateColor()
{
	static COLORREF curColor(0);
	const bool success(ColorUtil::GetColorizationColor(curColor));
	if (!success) return;
	UpdateColor(curColor);
}

// This function is called by the Windows function DispatchMessage()
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//	std::cout << "hwnd=" << hwnd << ", message=" << message << ", wParam=" << wParam << ", lParam=" << lParam << std::endl;

	// if explorer restarts, re-add tray icon
	// this can't be handled in the switch() below because it is a dynamic value
	if (message == WM_TASKBARCREATED)
	{
		Shell_NotifyIcon(NIM_ADD, &notifyIconData);
		return 0;
	}

	// handle the messages
	switch (message)         
	{
		// user-defined WM_SYSICON message indicating interaction with tray icon
		case WM_SYSICON:
		{
			// we could check for wParam == ID_TRAY_APP_ICON here, but why bother when we have only one?

			// check for mouse actions
			if (lParam == WM_LBUTTONUP)
			{
				GetAndUpdateColor();
			}
			else if (lParam == WM_RBUTTONUP)
			{
				POINT curPoint;
				GetCursorPos(&curPoint);
				SetForegroundWindow(hwnd); // some stupid trick that everyone does

				// TrackPopupMenu blocks the app until TrackPopupMenu returns
				const UINT clicked(TrackPopupMenu(Hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0, hwnd, NULL));
				switch (clicked)
				{
					case ID_TRAY_SYNC:  GetAndUpdateColor(); break; // manual color sync
					case ID_TRAY_ABOUT: ShowAbout(hwnd);     break; // show about menu
					case ID_TRAY_EXIT:  PostQuitMessage(0);  break; // quit the application
				}
			}
		}
		return 0;

		// accent color changed
		case WM_DWMCOLORIZATIONCOLORCHANGED:
		{
			UpdateColor(ColorUtil::Dword2ColorRef(wParam));
		}
		return 0;

		// session status changed (e.g. lock/unlock)
		case WM_WTSSESSION_CHANGE:
		{
			GetAndUpdateColor();
		}
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hThisInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpszArgument,
	int nCmdShow)
{
//	RedirectIOToConsole();
//	std::cout << "TEST" << std::endl;

	// register for taskbar re-create events so that icon can be restored after an explorer restart
	WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");

	// setup right-click popup menu
	Hmenu = CreatePopupMenu();
	AppendMenu(Hmenu, MF_STRING, ID_TRAY_SYNC, TEXT("Synchronize manually"));
	SetMenuDefaultItem(Hmenu, ID_TRAY_SYNC, FALSE);
	AppendMenu(Hmenu, MF_STRING, ID_TRAY_ABOUT, TEXT("About UniLight..."));
	AppendMenu(Hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));

	// create window prototype
	WNDCLASSEX wincl; // Data structure for the windowclass
	wincl.hInstance = hThisInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;
	wincl.style = CS_DBLCLKS;                 // Catch double-clicks
	wincl.cbSize = sizeof(WNDCLASSEX);
	wincl.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wincl.hIconSm = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;                 // No menu
	wincl.cbClsExtra = 0;                      // No extra bytes after the window class
	wincl.cbWndExtra = 0;                      // structure or the window instance
	wincl.hbrBackground = (HBRUSH)(CreateSolidBrush(RGB(255, 255, 255)));
	// register window prototype
	if (!RegisterClassEx(&wincl)) return 0;

	// create a dummy window to hang the tray icon from, and to process callbacks
	HWND hwnd(CreateWindowEx(
		0,
		szClassName,
		szClassName,
		WS_POPUP,
		0, 0, 0, 0,
		NULL,
		NULL,
		hThisInstance,
		NULL
	));

	WTSRegisterSessionNotification(hwnd, NOTIFY_FOR_ALL_SESSIONS);

	// initialize tray icon data structure
	memset(&notifyIconData, 0, sizeof(NOTIFYICONDATA));
	notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
	notifyIconData.hWnd = hwnd;
	notifyIconData.uID = ID_TRAY_APP_ICON;
	notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	notifyIconData.uCallbackMessage = WM_SYSICON; //Set up our invented Windows Message
	notifyIconData.hIcon = (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	StringCchCopy(notifyIconData.szTip, 128, szTIP);

	// add the tray icon to the system tray
	Shell_NotifyIcon(NIM_ADD, &notifyIconData);

	// perform initial LED color sync
	GetAndUpdateColor();

	// Run the UI message loop. It will run until GetMessage() returns 0
	MSG messages;
	while (GetMessage(&messages, NULL, 0, 0))
	{
		
		TranslateMessage(&messages); // Translate virtual-key messages into character messages		
		DispatchMessage(&messages);  // Send message to WindowProcedure
	}

//	std::cout << "TEST2" << std::endl;

	// we're done - clean up UI elements
	Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
	DestroyWindow(hwnd);
	DestroyMenu(Hmenu);

	return messages.wParam;
}
