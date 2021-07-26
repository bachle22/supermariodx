#pragma once

#define DIRECTINPUT_VERSION 0x0800

#pragma warning(push)
#pragma warning(disable: 26495)
#pragma warning(disable: 28251)
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#pragma warning( pop )

#include <unordered_map>
#include <windows.h>

#include "Input.h"
#include "Scene.h"

//constexpr float D3DX_PI = 3.141592654f;


enum RenderOptions
{
	NOFLIP = -1,
	FLIP = 1,
	VISIBLE = 255,
	INVISIBLE = 0,

	NOROTATE = 1,
	ROTATE180 = -1,
};

class Game
{
	static Game* __instance;
	HWND hWnd;												// Window handle

	LPDIRECT3D9 d3d = NULL;									// Direct3D handle
	LPDIRECT3DDEVICE9 d3ddev = NULL;						// Direct3D device object

	LPDIRECT3DSURFACE9 backBuffer = NULL;
	LPD3DXSPRITE spriteHandler = NULL;						// Sprite helper library to help us draw 2D image on the screen 

	LPDIRECTINPUT8 di{};										// The DirectInput object         
	LPDIRECTINPUTDEVICE8 didev{};								// The keyboard device 
	BYTE keyStates[256]{};									// DirectInput keyboard state buffer 
	DIDEVICEOBJECTDATA keyEvents[KEYBOARD_BUFFER_SIZE]{};		// Buffered keyboard data
	LPKEYEVENTHANDLER keyHandler{};

	int screenWidth{};
	int screenHeight{};

	std::unordered_map<int, LPSCENE> scenes;
	int currentScene{};
	int lastScene{};

	bool isPaused{};


public:
	int DEBUG_X = 0, DEBUG_Y = 0;

	void Init(HWND hWnd);
	void InitKeyboard();
	void SetKeyHandler(LPKEYEVENTHANDLER handler) { keyHandler = handler; }

	void Draw(float x, float y,
		LPDIRECT3DTEXTURE9 texture,
		int left, int top, int right, int bottom);
	void Draw(int nx, float x, float y,
		LPDIRECT3DTEXTURE9 texture,
		int left, int top, int right, int bottom,
		int alpha = 255);
	void Draw(int nx, float x, float y,
		LPDIRECT3DTEXTURE9 texture,
		int left, int top, int right, int bottom,
		int alpha, D3DXVECTOR2 translation);
	void Draw(int nx, float x, float y,
		LPDIRECT3DTEXTURE9 texture,
		int left, int top, int right, int bottom,
		int alpha, D3DXVECTOR2 translation, int rotation);

	int IsKeyDown(int KeyCode);
	void ProcessKeyboard();

	void _ParseSection_SETTINGS(std::string line);
	void _ParseSection_SCENES(std::string line);

	void Load(LPCWSTR gameFile);
	LPSCENE GetCurrentScene() { return scenes[currentScene]; }
	void SwitchScene(int scene_id);
	void FastSwitchScene(int scene_id);

	int GetScreenWidth() { return screenWidth; }
	int GetScreenHeight() { return screenHeight; }
	bool IsPaused() { return isPaused; }
	void Pause() { isPaused = true; }
	void Unpause() { isPaused = false; }

	LPDIRECT3DDEVICE9 GetDirect3DDevice() { return this->d3ddev; }
	LPDIRECT3DSURFACE9 GetBackBuffer() { return backBuffer; }
	LPD3DXSPRITE GetSpriteHandler() { return this->spriteHandler; }

	static Game* GetInstance();

	~Game();
};