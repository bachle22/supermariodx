#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "Animations.h"
#include "Debug.h"
#include "Definition.h"
#include "Game.h"
#include "Mario.h"
#include "Textures.h"
#include "Sprites.h"

#define BACKGROUND_COLOR D3DCOLOR_XRGB(200, 200, 255)

Mario* mario;
#define MARIO_START_X 10.0f
#define MARIO_START_Y 130.0f
#define MARIO_START_VX 0.1f

#define ID_TEX_MARIO 0
#define ID_TEX_ENEMY 10
#define ID_TEX_MISC 20

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
	Textures* textures = Textures::GetInstance();

	textures->Add(ID_TEX_MARIO, L"..\\assets\\textures\\mario.png", D3DCOLOR_XRGB(176, 224, 248));


	Sprites* sprites = Sprites::GetInstance();

	LPDIRECT3DTEXTURE9 texMario = textures->Get(ID_TEX_MARIO);

	sprites->Add(10001, 246, 154, 259, 181, texMario);
	sprites->Add(10002, 275, 154, 290, 181, texMario);
	sprites->Add(10003, 304, 154, 321, 181, texMario);

	sprites->Add(10011, 186, 154, 199, 181, texMario);
	sprites->Add(10012, 155, 154, 170, 181, texMario);
	sprites->Add(10013, 125, 154, 140, 181, texMario);

	LPDIRECT3DTEXTURE9 texMisc = textures->Get(ID_TEX_MISC);
	sprites->Add(20001, 300, 117, 315, 132, texMisc);
	sprites->Add(20002, 318, 117, 333, 132, texMisc);
	sprites->Add(20003, 336, 117, 351, 132, texMisc);
	sprites->Add(20004, 354, 117, 369, 132, texMisc);


	Animations* animations = Animations::GetInstance();
	LPANIMATION ani;

	ani = new Animation(100);
	ani->Add(10001);
	ani->Add(10002);
	ani->Add(10003);
	animations->Add(500, ani);

	ani = new Animation(100);
	ani->Add(10011);
	ani->Add(10012);
	ani->Add(10013);
	animations->Add(501, ani);

	
	ani = new Animation(100);
	ani->Add(20001,1000);
	ani->Add(20002);
	ani->Add(20003);
	ani->Add(20004);
	animations->Add(510, ani);

	mario = new Mario();
	Mario::AddAnimation(500);
	Mario::AddAnimation(501);
	mario->SetPosition(MARIO_START_X, MARIO_START_Y);

}

void Render(void)
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

	mario->Render();

	spriteHandler->End();
	d3ddev->EndScene();

	d3ddev->Present(NULL, NULL, NULL, NULL);    // Displays the created frame
}

void Update(DWORD dt)
{
	mario->Update(dt);
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
	SetDebugWindow(hWnd);

	Game* game = Game::GetInstance();
	game->Init(hWnd);
	LoadResources();
	HandleWindowMessage();

}

