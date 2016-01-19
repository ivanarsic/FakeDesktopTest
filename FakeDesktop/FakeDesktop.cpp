// FakeDesktop.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include <iostream>
#include <stdexcept>
#include <gdiplus.h>
#pragma comment(lib,"gdiplus.lib")

using namespace std;
using namespace Gdiplus;

const LPCWSTR wndClassName = L"Fake Desktop Class";

static LPCWSTR bitmapLocation;

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP bmpSource = NULL;
	static HDC hdcSource = NULL;
	PAINTSTRUCT ps;
	HDC hdcDestination;

	HBITMAP hBmpBck = NULL;
	switch (msg)
	{
	case WM_CREATE:
	{
		//Load a jpeg file (or any picture file supported by GDI+)
		Bitmap mBitmap(bitmapLocation, false);
		mBitmap.GetHBITMAP(0x00000000, &bmpSource);
		// Getting a handler for device context for bitmap data
		hdcSource = CreateCompatibleDC(GetDC(0));
		SelectObject(hdcSource, bmpSource);
		break;
	}
	case WM_CLOSE:
	{
		DeleteObject(bmpSource);
		DeleteDC(hdcSource);
		DestroyWindow(hwnd);
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	case WM_PAINT:
	{
		if (bmpSource != NULL)
		{
			// Getting a HDC for the display and preparing for paining
			hdcDestination = BeginPaint(hwnd, &ps);
			// "Painting" the bitmap to the display
			BitBlt(hdcDestination, 0, 0, 1024, 768, hdcSource, 0, 0, SRCCOPY);
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

	__declspec(dllexport) void __stdcall ShowDesktopWindow(const wchar_t *fakeDesktopLocation)
	{	
		bitmapLocation = fakeDesktopLocation;
		//Starting GDI+ (used for loading jpeg)
		GdiplusStartupInput gdiplusStartupInput;
		ULONG_PTR gdiplusToken;
		GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
		//creating and registering window class (to be used for Faking the backround
		WNDCLASSEX wndclass = { sizeof(WNDCLASSEX), CS_DBLCLKS, WindowProcedure,
			0, 0, GetModuleHandle(0), LoadIcon(0, IDI_APPLICATION),
			LoadCursor(0, IDC_ARROW), HBRUSH(COLOR_WINDOW + 1),
			0, wndClassName, LoadIcon(0, IDI_APPLICATION) };
		if (RegisterClassEx(&wndclass))
		{
			//setting the size and style of the window to POP_UP to remove border 
			//and positioning the window as top most and maximized
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
		//Closing GDI+
		GdiplusShutdown(gdiplusToken);
	}

	__declspec(dllexport) void __stdcall KillDesktopWindow()
	{
		// Finding the handler for the window used for faking the desktop
		HWND window = FindWindow(wndClassName, 0);
		if (window == NULL)
			return;
		LRESULT r = SendMessage(window, WM_SYSCOMMAND, SC_CLOSE, 0);
	}
}


