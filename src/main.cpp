#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "Debug.h"
#include "Definition.h"
#include "Game.h"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(200, 200, 255)

Game* game;

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

void Render()
{
	Game* game = Game::GetInstance();
	LPDIRECT3DDEVICE9 d3ddev = game->GetDirect3DDevice();
	LPDIRECT3DSURFACE9 backbuffer = game->GetBackBuffer();
	LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();


	// Clear out back buffer
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, BACKGROUND_COLOR, 1.0f, 0);
	d3ddev->BeginScene();

	// Render sprites & animation
	spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);

	Game::GetInstance()->GetCurrentScene()->Render();

	spriteHandler->End();
	d3ddev->EndScene();

	d3ddev->Present(NULL, NULL, NULL, NULL);    // Displays the created frame
}

/*
	Update world status for this frame
	dt: time period between beginning of last frame and beginning of this frame
*/
void Update(ULONGLONG dt)
{
	Game::GetInstance()->GetCurrentScene()->Update(dt);
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
		CW_USEDEFAULT,			// X-position of the window
		CW_USEDEFAULT,			// Y-position of the window
		ScreenWidth,
		ScreenHeight,
		NULL,					// No parent window, NULL
		NULL,					// No menu, NULL
		hInstance,
		NULL);

	if (!hWnd)
	{
		ULONGLONG ErrCode = GetLastError();
		DebugOut(L"[ERROR] CreateWindow failed");
		return 0;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return hWnd;
}

WPARAM HandleWindowMessage()
{
	bool isDone = 0;
	MSG msg;
	ULONGLONG frameStart = GetTickCount64();
	ULONGLONG tickPerFrame = 1000 / MAX_FRAME_RATE;

	while (!isDone)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			switch (msg.message)
			{
			case WM_QUIT:
				isDone = 1;
				break;
			case WM_SIZING:
				// TODO: Handle window resizing
				break;
			case WM_ENTERSIZEMOVE:
				DebugOut(L"PAUSED\n");
				break;
			}
		}

		ULONGLONG now = GetTickCount64();
		//DebugOut(L"%llu\n", now);

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		ULONGLONG dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;
			game->ProcessKeyboard();
			Update(dt);
			Render();
		}
	}
	return msg.wParam;
}


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

	game = Game::GetInstance();
	game->Init(hWnd);
	game->InitKeyboard();
	game->Load(L"..\\scenes\\main.config");

	SetWindowPos(hWnd, 0, 0, 0, 
		SCREEN_WIDTH * SIZE_MULTIPLIER, SCREEN_HEIGHT * SIZE_MULTIPLIER, 
		SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
	HandleWindowMessage();

}

