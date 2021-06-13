#pragma once
#include "Debug.h"
#include "Game.h"

CGame* CGame::__instance = NULL;

void CGame::Init(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	this->hWnd = hWnd;
	D3DPRESENT_PARAMETERS d3dpp;				// Struct to hold various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp));			// Clear out the struct for use
	d3dpp.Windowed = TRUE;						// Program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// Discard old frames
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;	// 32-bit RGB pixel format, where 8 bits are reserved for each color.
	d3dpp.hDeviceWindow = hWnd;					// Set the window to be used by DirectX
	d3dpp.BackBufferCount = 1;

	// Retrieve window width & height so that we can create backbuffer height & width accordingly 
	RECT r;
	GetClientRect(hWnd, &r);

	d3dpp.BackBufferHeight = r.bottom + 1;
	d3dpp.BackBufferWidth = r.right + 1;

	backBufferWidth = d3dpp.BackBufferWidth;
	backBufferHeight = d3dpp.BackBufferHeight;

	// Create a device class using this information and information from the d3dpp stuct
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,						// Use default GPU
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,	// D3DCREATE_SOFTWARE_VERTEXPROCESSING is not recommended since Windows version 1607
		&d3dpp,
		&d3ddev);

	if (d3ddev == NULL)
	{
		DebugOut(L"[ERROR] CreateDevice failed\n");
		return;
	}

	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backBuffer);

	// Initialize sprite helper from Direct3DX helper library
	D3DXCreateSprite(d3ddev, &spriteHandler);

	DebugOut(L"[INFO] InitGame done;\n");
}

/*
	Utility function to wrap LPD3DXSPRITE::Draw
*/
void CGame::Draw(float x, float y, LPDIRECT3DTEXTURE9 texture)
{
	D3DXVECTOR3 p(x, y, 0);
	spriteHandler->Draw(texture, NULL, NULL, &p, D3DCOLOR_XRGB(255, 255, 255));
}

/*
	Utility function to wrap LPD3DXSPRITE::Draw
*/
void CGame::Draw(float x, float y, LPDIRECT3DTEXTURE9 texture, int left, int top, int right, int bottom)
{
	D3DXVECTOR3 p(x, y, 0);
	RECT r;
	r.left = left;
	r.top = top;
	r.right = right;
	r.bottom = bottom;
	spriteHandler->Draw(texture, &r, NULL, &p, D3DCOLOR_XRGB(255, 255, 255));
}

/*
	Utility function to wrap D3DXCreateTextureFromFileEx
*/
LPDIRECT3DTEXTURE9 CGame::LoadTexture(LPCWSTR texturePath)
{
	LPDIRECT3DTEXTURE9 texture;

	LPDIRECT3DDEVICE9 d3ddev = CGame::GetInstance()->GetDirect3DDevice();

	HRESULT result = D3DXCreateTextureFromFileEx(
		d3ddev,									// Pointer to Direct3D device object
		texturePath,							// Path to the image to load
		D3DX_DEFAULT_NONPOW2,					// Texture width
		D3DX_DEFAULT_NONPOW2,					// Texture height
		1,
		D3DUSAGE_DYNAMIC,
		D3DFMT_UNKNOWN,
		D3DPOOL_DEFAULT,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DCOLOR_XRGB(255, 255, 255),			// Transparent color
		NULL,
		NULL,
		&texture);								// Created texture pointer

	if (result != D3D_OK)
	{
		DebugOut(L"[ERROR] CreateTextureFromFile failed. File: %s\n");
		return NULL;
	}

	DebugOut(L"[INFO] Texture loaded Ok from file: %s \n");
	return texture;
}

CGame::~CGame()
{
	if (spriteHandler != NULL) spriteHandler->Release();
	if (backBuffer != NULL) backBuffer->Release();
	if (d3ddev != NULL) d3ddev->Release();
	if (d3d != NULL) d3d->Release();
}

CGame* CGame::GetInstance()
{
	if (__instance == NULL) __instance = new CGame();
	return __instance;
}
