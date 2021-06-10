#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#include "Debug.h"
#include "Definition.h"
#include "Game.h"
#include "GameObject.h"

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

// Set up and initialize DirectX
//void InitDirectX(HWND hWnd)
//{
//	d3d = Direct3DCreate9(D3D_SDK_VERSION);		// Create the Direct3D interface
//
//	D3DPRESENT_PARAMETERS d3dpp;				// Struct to hold various device information
//
//	ZeroMemory(&d3dpp, sizeof(d3dpp));			// Clear out the struct for use
//	d3dpp.Windowed = TRUE;						// Program windowed, not fullscreen
//	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// Discard old frames
//	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;	// 32-bit RGB pixel format, where 8 bits are reserved for each color.
//	d3dpp.hDeviceWindow = hWnd;					// Set the window to be used by DirectX
//
//	// Retrieve window width & height so that we can create backbuffer height & width accordingly 
//	RECT r;
//	GetClientRect(hWnd, &r);
//
//	BackBufferWidth = r.right + 1;
//	BackBufferHeight = r.bottom + 1;
//
//	d3dpp.BackBufferHeight = BackBufferHeight;
//	d3dpp.BackBufferWidth = BackBufferWidth;
//
//	// Create a device class using this information and information from the d3dpp stuct
//	d3d->CreateDevice(
//		D3DADAPTER_DEFAULT,						// Use default GPU
//		D3DDEVTYPE_HAL,
//		hWnd,
//		D3DCREATE_HARDWARE_VERTEXPROCESSING,	// D3DCREATE_SOFTWARE_VERTEXPROCESSING is not recommended since Windows version 1607
//		&d3dpp,
//		&d3ddev);
//
//	if (d3ddev == NULL)
//	{
//		OutputDebugString(L"[ERROR] CreateDevice failed\n");
//		return;
//	}
//
//	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);
//
//	// Initialize Direct3DX helper library
//	D3DXCreateSprite(d3ddev, &spriteHandler);
//}

//void LoadResources(void)
//{
//	HRESULT result = D3DXCreateTextureFromFileEx(
//		d3ddev,								// Pointer to Direct3D device object
//		BRICK_TEXTURE_PATH,					// Path to the image to load
//		D3DX_DEFAULT_NONPOW2, 				// Auto texture width (get from file)
//		D3DX_DEFAULT_NONPOW2, 				// Auto texture height (get from file)
//		1,
//		D3DUSAGE_DYNAMIC,
//		D3DFMT_UNKNOWN,
//		D3DPOOL_DEFAULT,
//		D3DX_DEFAULT,
//		D3DX_DEFAULT,
//		D3DCOLOR_XRGB(255, 255, 255),		// Transparent color
//		NULL,
//		NULL,
//		&texBrick);
//}

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
		OutputDebugString(L"[ERROR] CreateWindow failed");
		return 0;
	}
	ShowWindow(hWnd, nCmdShow);
	return hWnd;
}

WPARAM HandleWindowMessage(void)
{
	MSG msg;

	DWORD frameStart = GetTickCount();
	DWORD tickPerFrame = 1000 / MAX_FRAME_RATE;
	while (TRUE)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT) break;
		Render();
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

