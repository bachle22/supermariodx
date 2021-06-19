#pragma once
#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <windows.h>

#define DIRECTINPUT_VERSION 0x0800
#define KEYBOARD_BUFFER_SIZE 1024

#define KEYBOARD_BUFFER_SIZE 1024
/*
Abstract class to define keyboard event handlers
*/
class CKeyEventHandler
{
public:
	virtual void KeyState(BYTE* state) = 0;
	virtual void OnKeyDown(int KeyCode) = 0;
	virtual void OnKeyUp(int KeyCode) = 0;
};

typedef CKeyEventHandler* LPKEYEVENTHANDLER;

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
	BYTE  keyStates[256];									// DirectInput keyboard state buffer 
	DIDEVICEOBJECTDATA keyEvents[KEYBOARD_BUFFER_SIZE];		// Buffered keyboard data

public:
	void Init(HWND hWnd);
	void InitKeyboard(LPKEYEVENTHANDLER handler);
	void Draw(float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom);

	LPDIRECT3DDEVICE9 GetDirect3DDevice() { return this->d3ddev; }
	LPDIRECT3DSURFACE9 GetBackBuffer() { return backBuffer; }
	LPD3DXSPRITE GetSpriteHandler() { return this->spriteHandler; }

	static Game* GetInstance();

	~Game();
};