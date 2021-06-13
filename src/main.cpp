#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "Debug.h"
#include "Definition.h"
#include "Game.h"
#include "GameObject.h"

using namespace std;

#define BRICK_TEXTURE_PATH L"brick.png"
#define MARIO_TEXTURE_PATH L"mario.png"

CMario* mario;
#define MARIO_START_X 10.0f
#define MARIO_START_Y 130.0f
#define MARIO_START_VX 0.1f

CGameObject* brick;
#define BRICK_X 10.0f
#define BRICK_Y 100.0f

LPDIRECT3DTEXTURE9 texMario = NULL;
LPDIRECT3DTEXTURE9 texBrick = NULL;

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

void LoadResources()
{
	CGame* game = CGame::GetInstance();
	texBrick = game->LoadTexture(BRICK_TEXTURE_PATH);
	texMario = game->LoadTexture(MARIO_TEXTURE_PATH);

	mario = new CMario(MARIO_START_X, MARIO_START_Y, MARIO_START_VX, texMario);
	brick = new CGameObject(BRICK_X, BRICK_Y, texBrick);
}

void Render(void)
{

	CGame* game = CGame::GetInstance();
	LPDIRECT3DDEVICE9 d3ddev = game->GetDirect3DDevice();
	LPDIRECT3DSURFACE9 bb = game->GetBackBuffer();
	LPD3DXSPRITE spriteHandler = game->GetSpriteHandler();
	// Clear out back buffer
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_BLACK, 1.0f, 0);
	d3ddev->BeginScene();

	// Render sprites & animation
	spriteHandler->Begin(D3DXSPRITE_ALPHABLEND);

	mario->Render();
	brick->Render();

	spriteHandler->End();
	d3ddev->EndScene();
	d3ddev->Present(NULL, NULL, NULL, NULL);    // Displays the created frame
}

/*
	Update world status for this frame
	dt: time period between beginning of last frame and beginning of this frame
*/
void Update(DWORD dt)
{
	/*
	for (int i=0;i<n;i++)
		objects[i]->Update(dt);
	*/

	mario->Update(dt);
	brick->Update(dt);

	DebugOut(L"01 - Skeleton %0.1f, %0.1f \n", mario->GetX(), mario->GetY());
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
		DWORD ErrCode = GetLastError();
		DebugOutTitle(L"[ERROR] CreateWindow failed");
		return 0;
	}
	ShowWindow(hWnd, nCmdShow);
	return hWnd;
}

WPARAM HandleWindowMessage(void)
{
	bool isDone = 0;
	MSG msg;
	DWORD frameStart = GetTickCount64();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;

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
			}
		}

		DWORD now = GetTickCount64();

		// dt: the time between (beginning of last frame) and now
		// this frame: the frame we are about to render
		DWORD dt = now - frameStart;

		if (dt >= tickPerFrame)
		{
			frameStart = now;
			Update(dt);
			Render();
		}
		else Sleep(tickPerFrame - dt);
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

	if (hWnd == 0) return 0;

	CGame* game = CGame::GetInstance();
	game->Init(hWnd);
	LoadResources();
	HandleWindowMessage();

}

