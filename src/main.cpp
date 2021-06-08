#include <windows.h>

#include "Definition.h"
#include "Debug.h"

// Window Message Processor
LRESULT CALLBACK WindowProc(
	_In_ HWND hWnd,
	_In_ UINT message,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND CreateGameWindow(
	HINSTANCE hInstance,
	int nCmdShow,
	LPCWSTR WindowClassName,
	LPCWSTR WindowTitle,
	int ScreenWidth,
	int ScreenHeight)
{
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.hInstance = hInstance;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = WindowClassName;

	RegisterClassEx(&wc);

	HWND hWnd = CreateWindow(
		WindowClassName,
		WindowTitle,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,		// X-position of the window
		CW_USEDEFAULT,		// Y-position of the window
		ScreenWidth,
		ScreenHeight,
		NULL,		// No parent window, NULL
		NULL,		// No menu, NULL
		hInstance,
		NULL);

	if (!hWnd)
	{
		DWORD ErrCode = GetLastError();
		OutputDebugString(L"[ERROR] CreateWindow failed");
		return 0;
	}
	ShowWindow(hWnd, nCmdShow);
	return hWnd;
}


// Entry point
int WINAPI WinMain(
	_In_     HINSTANCE	hInstance,
	_In_opt_ HINSTANCE	hPrevInstance,
	_In_     LPSTR		lpCmdLine,
	_In_     int		nCmdShow)
{
	HWND hWnd = CreateGameWindow(
		hInstance, nCmdShow,
		WINDOW_CLASS_NAME, MAIN_WINDOW_TITLE,
		SCREEN_WIDTH, SCREEN_HEIGHT);
	if (hWnd == 0) return 0;

	MSG msg;
	// Window message loop
	while (TRUE)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT) break;


	}

	return msg.wParam;
}

