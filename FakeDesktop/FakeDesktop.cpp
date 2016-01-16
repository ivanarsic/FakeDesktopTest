// FakeDesktop.cpp : Defines the exported functions for the DLL application.
//
#include "stdafx.h"
#include <iostream>
#include <stdexcept>

using namespace std;

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
}
