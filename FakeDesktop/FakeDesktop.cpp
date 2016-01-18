// FakeDesktop.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include <iostream>
#include <stdexcept>

using namespace std;

const LPCWSTR wndClassName = L"Fake Desktop Class";

const LPCWSTR wndName = L"Fake Desktop";

const wchar_t* bitmapLocation = L"C:\\Kiosk\\background.jpg";
//
//HDC hdcMem = NULL;
//HBITMAP hBmp = NULL;
//HBITMAP hBmpOld = NULL;

//void LoadPic()
//{
//	if (hdcMem)
//		return;  // already loaded, no need to load it again
//
//	// note:  here you must put the file name in a TEXT() macro.  DO NOT CAST IT TO LPCSTR
//	hBmp = (HBITMAP)LoadImage(NULL, bitmapLocation, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
//
//	if (!hBmp)  // the load failed (couldn't find file?  Invalid file?)
//		return;
//
//	hdcMem = CreateCompatibleDC(NULL);
//	hBmpOld = (HBITMAP)SelectObject(hdcMem, hBmp);
//}

// call this function when the program is done (shutting down)
//void FreePic()
//{
//	if (hdcMem)
//	{
//		SelectObject(hdcMem, hBmpOld);
//		DeleteObject(hBmp);
//		DeleteDC(hdcMem);
//	}
//}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	HBITMAP hBmpBck = NULL;
	switch (msg)
	{
	case WM_CREATE:
	{
		hBmpBck = (HBITMAP)LoadImage(NULL, bitmapLocation, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
		break;
	}
	case WM_CLOSE:
	{
		//FreePic();
		DestroyWindow(hwnd);
		break;
	}
	case WM_DESTROY:
	{
		DeleteObject(hBmpBck);
		PostQuitMessage(0);
		break;
	}
	case WM_PAINT:
	{
		if (hBmpBck != NULL)
		{
			BITMAP bm;
			PAINTSTRUCT ps;

			HDC hdc = BeginPaint(hwnd, &ps);

			HDC hdcMem = CreateCompatibleDC(hdc);
			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hBmpBck);

			GetObject(hBmpBck, sizeof(bm), &bm);

			BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteDC(hdcMem);

			EndPaint(hwnd, &ps);
		}
		
		break;
	}
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

EXTERN_C
{
	__declspec(dllexport) void __stdcall GetScreenResolution(int& horizontal, int& vertical)
	{
		RECT desktop;
		// Get a handle to the desktop window
		const HWND hDesktop = GetDesktopWindow();
		// Get the size of screen to the variable desktop
		GetWindowRect(hDesktop, &desktop);
		// The top left corner will have coordinates (0,0)
		// and the bottom right corner will have coordinates
		// (horizontal, vertical)
		horizontal = desktop.right;
		vertical = desktop.bottom;
	}

	__declspec(dllexport) bool __stdcall SetScreenResolution(int width, int height)
	{
		DEVMODE originalMode = { 0 };

		// Retrieving current settings
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &originalMode);

		// Copy the current settings to new for editing
		DEVMODE newMode = originalMode;

		// Changing the settings
		newMode.dmPelsWidth = (unsigned long)width;
		newMode.dmPelsHeight = (unsigned long)height;

		// Capturing the operation result
		int result = ChangeDisplaySettings(&newMode, 0);
		return result == DISP_CHANGE_SUCCESSFUL;
	}

	__declspec(dllexport) void __stdcall ShowDesktopWindow()
	{	

		WNDCLASSEX wndclass = { sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure,
			0, 0, GetModuleHandle(0), LoadIcon(0, IDI_APPLICATION),
			LoadCursor(0, IDC_ARROW), HBRUSH(COLOR_WINDOW + 1),
			0, wndClassName, LoadIcon(0, IDI_APPLICATION) };
		if (RegisterClassEx(&wndclass))
		{
			HWND window = CreateWindowEx(0, wndClassName, 0,
				WS_POPUP, 0, 0,
				1024, 768, 0, 0, GetModuleHandle(0), 0);
			SetWindowLong(window, GWL_STYLE, WS_POPUP);
			SetWindowPos(window, HWND_TOPMOST, 0, 0, 1024, 768, SWP_FRAMECHANGED);
			if (window)
			{
				ShowWindow(window, SW_MAXIMIZE);
				MSG msg;
				while (GetMessage(&msg, 0, 0, 0)) DispatchMessage(&msg);
			}
		}


	}

	__declspec(dllexport) void __stdcall KillDesktopWindow()
	{
		HWND window = FindWindow(wndClassName, 0);
		if (window == NULL)
			return;
		LRESULT r=SendMessage(window, WM_SYSCOMMAND, SC_CLOSE, 0);
	}
}


