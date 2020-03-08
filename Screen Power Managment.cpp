// Screen Power Managment.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Screen Power Managment.h"
#include "powerbase.h"
#include "shellapi.h"
#include "combaseapi.h"
#include "strsafe.h"
#include "Commctrl.h"

//#pragma comment(lib,"Comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

NOTIFYICONDATA nid = {};


bool applyVideoTimeout(/*DWORD newtimeOut*/)
{
	SYSTEM_POWER_POLICY powerPolicy;
	DWORD ret;
	DWORD size = sizeof(SYSTEM_POWER_POLICY);

	ret = CallNtPowerInformation(SystemPowerPolicyAc, nullptr, 0, &powerPolicy, size);

	if ((ret != ERROR_SUCCESS) || (size != sizeof(SYSTEM_POWER_POLICY)))
	{
		return false;
	}
	
	DWORD cur_timeOut = powerPolicy.VideoTimeout;

	DWORD newtimeOut = 60;

	if (cur_timeOut > 60)
	{
		newtimeOut = 60;
	}
	else
	{
		newtimeOut = 15 * 60;
	}

	powerPolicy.VideoTimeout = newtimeOut;
	ret = CallNtPowerInformation(SystemPowerPolicyAc, &powerPolicy, size, nullptr, 0);

	if ((ret != ERROR_SUCCESS))
	{
		return false;
	}

	return true;
}

DWORD getVideoTimeout()
{
	SYSTEM_POWER_POLICY powerPolicy;
	DWORD ret;
	DWORD size = sizeof(SYSTEM_POWER_POLICY);

	ret = CallNtPowerInformation(SystemPowerPolicyAc, nullptr, 0, &powerPolicy, size);

	if ((ret != ERROR_SUCCESS) || (size != sizeof(SYSTEM_POWER_POLICY)))
	{
		return -1;
	}

	return powerPolicy.VideoTimeout;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    applyVideoTimeout(/*90*/);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SCREENPOWERMANAGMENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCREENPOWERMANAGMENT));

    MSG msg;
	
	// Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SCREENPOWERMANAGMENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SCREENPOWERMANAGMENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
		
    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      1440/2, 1440 / 2, 300, 300, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // Do NOT set the NIF_INFO flag.

   nid.uVersion = NOTIFYICON_VERSION_4;
   nid.cbSize = sizeof(nid);
   nid.hWnd = hWnd;
   nid.uFlags = NIF_ICON | NIF_TIP | NIF_GUID;

   // Note: This is an example GUID only and should not be used.
   // Normally, you should use a GUID-generating tool to provide the value to
   // assign to guidItem.
   GUID gidReference;
   HRESULT hCreateGuid = CoCreateGuid(&gidReference);

   //nid.guidItem = gidReference;

   // This text will be shown as the icon's tooltip.
   StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), L"Test application");

   // Load the icon for high DPI.
   LoadIconMetric(hInst, MAKEINTRESOURCE(IDI_SMALL), LIM_SMALL, &(nid.hIcon));

   //// Show the notification.
   Shell_NotifyIcon(NIM_ADD, &nid) ? S_OK : E_FAIL;

   //// Set the version
   Shell_NotifyIcon(NIM_SETVERSION, &nid);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
			//wchar_t *text;
			//text = new wchar_t[500];
			//
			//DWORD currentTimeOut = getVideoTimeout();

			////_ultot(currentTimeOut, text, 10);
			////_ultow_s(currentTimeOut, text, sizeof(DWORD));
			////TCHAR greeting[] = _T("Hello, Windows desktop!");
			////TCHAR greeting[] = _T(&text);
			//wchar_t greeting[] = L"123";
			//TextOut(hdc,
			//	5, 5,
			//	greeting, _tcslen(greeting));

			//delete &text;

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);

        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}