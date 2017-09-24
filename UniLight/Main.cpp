// UniLight by HunterZ

#include "ColorUtil.h"
#include "LFXUtil.h"
#include "LLEDUtil.h"
#include "resource.h"

#include <iomanip>
#include <sstream>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>

#define ID_TRAY_APP_ICON 1001
#define ID_TRAY_EXIT     1002
#define ID_TRAY_ABOUT    1003
#define IDT_TIMER        1004
#define IDT_TIMER_INIT   1005
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

void ShowAbout(HWND hwnd)
{
	SetForegroundWindow(hwnd);
	static bool active(false);
	if (active) return; // don't allow multiple about popups to be opened
	active = true;
	MessageBox(hwnd, _T("UniLight by HunterZ (https://github.com/HunterZ/UniLight)"), _T("About UniLight"), NULL);
	active = false;
}

// This function is called by the Windows function DispatchMessage()
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
		// user-defined WM_SYSICON message
		case WM_SYSICON:
		{
			// we could check for wParam == ID_TRAY_APP_ICON here, but why bother when we have only one?

			// check for mouse actions
			if (lParam == WM_LBUTTONUP)
			{
				ShowAbout(hwnd);
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
					case ID_TRAY_EXIT:  PostQuitMessage(0); break; // quit the application
					case ID_TRAY_ABOUT:	ShowAbout(hwnd);    break; // show about menu
				}
			}
		}
		break;

		// color check timers
		case WM_TIMER:
		{
			COLORREF curColor(0);
			const bool success(ColorUtil::GetColorizationColor(curColor));
			static bool firstTime(true);
			if (success &&
			    (curColor != lastColor || firstTime))
			{
				firstTime = false;

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
			}

			if (wParam == IDT_TIMER_INIT)
			{
				// this is the initial one-shot timer
				// kill it and start the regular timer
				KillTimer(hwnd, IDT_TIMER_INIT);
				SetTimer(hwnd, IDT_TIMER, 1000, (TIMERPROC)NULL);
			}
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
	// register for taskbar re-create events so that icon can be restored after an explorer restart
	WM_TASKBARCREATED = RegisterWindowMessageA("TaskbarCreated");

	// setup right-click popup menu
	Hmenu = CreatePopupMenu();
	AppendMenu(Hmenu, MF_STRING, ID_TRAY_ABOUT, TEXT("About UniLight..."));
	SetMenuDefaultItem(Hmenu, ID_TRAY_ABOUT, FALSE);
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

	// kick off an immediate one-shot timer to seed color processing
	SetTimer(hwnd,             // handle to main window 
	         IDT_TIMER_INIT,   // timer identifier 
	         0,                // interval in milliseconds
	         (TIMERPROC)NULL); // no timer callback (WindowProcedure() will be invoked)

	// Run the UI message loop. It will run until GetMessage() returns 0
	MSG messages;
	while (GetMessage(&messages, NULL, 0, 0))
	{
		
		TranslateMessage(&messages); // Translate virtual-key messages into character messages		
		DispatchMessage(&messages);  // Send message to WindowProcedure
	}

	// we're done - clean up UI elements
	Shell_NotifyIcon(NIM_DELETE, &notifyIconData);
	DestroyWindow(hwnd);
	DestroyMenu(Hmenu);

	return messages.wParam;
}
