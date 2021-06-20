#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <windows.h>

#include "Input.h"

class Game
{
	static Game* __instance;
	HWND hWnd;												// Window handle

	LPDIRECT3D9 d3d = NULL;									// Direct3D handle
	LPDIRECT3DDEVICE9 d3ddev = NULL;						// Direct3D device object

	LPDIRECT3DSURFACE9 backBuffer = NULL;
	LPD3DXSPRITE spriteHandler = NULL;						// Sprite helper library to help us draw 2D image on the screen 

	LPDIRECTINPUT8 di;										// The DirectInput object         
	LPDIRECTINPUTDEVICE8 didev;								// The keyboard device 
	BYTE keyStates[256];									// DirectInput keyboard state buffer 
	DIDEVICEOBJECTDATA keyEvents[KEYBOARD_BUFFER_SIZE];		// Buffered keyboard data
	LPKEYEVENTHANDLER keyHandler;

	float cam_x = 0.0f;
	float cam_y = 0.0f;

public:
	void Init(HWND hWnd);
	void InitKeyboard(LPKEYEVENTHANDLER handler);
	void Draw(float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom, int alpha = 255);

	int IsKeyDown(int KeyCode);
	void ProcessKeyboard();

	static void SweptAABB(
		float ml,			// move left 
		float mt,			// move top
		float mr,			// move right 
		float mb,			// move bottom
		float dx,			// 
		float dy,			// 
		float sl,			// static left
		float st,
		float sr,
		float sb,
		float& t,
		float& nx,
		float& ny);

	LPDIRECT3DDEVICE9 GetDirect3DDevice() { return this->d3ddev; }
	LPDIRECT3DSURFACE9 GetBackBuffer() { return backBuffer; }
	LPD3DXSPRITE GetSpriteHandler() { return this->spriteHandler; }

	void SetCamPos(float x, float y) { cam_x = x; cam_y = y; }

	static Game* GetInstance();

	~Game();
};