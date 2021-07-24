#pragma once

#pragma warning(push, 0)
#pragma warning(disable: ALL_CODE_ANALYSIS_WARNINGS)
#include <d3dx9.h>
#pragma warning( pop )

#include <windows.h>
#include <unordered_map>

constexpr int DISABLED_SPRITE_ID = 0;

class Sprite
{
	int id;				// Sprite ID in the sprite database

	int left;
	int top;
	int right;
	int bottom;

	LPDIRECT3DTEXTURE9 texture;
public:
	Sprite(int id, int left, int top, int right, int bottom, LPDIRECT3DTEXTURE9 tex);
	void Draw(int nx, float x, float y, int alpha, D3DXVECTOR2 translation, int rotation);
	void Draw(int nx, float x, float y, int alpha);
	void Draw(int nx, float x, float y, int alpha, int width, int height);
	void Draw(int nx, float x, float y);
	void Draw(float x, float y);
	void Draw(int nx, float x, float y, 
		int alpha, D3DXVECTOR2 translation, 
		int width, int height);
};

typedef Sprite* LPSPRITE;

/*
	Manage sprite database
*/
class Sprites
{
	static Sprites* __instance;

	std::unordered_map<int, LPSPRITE> sprites;

public:
	void Add(int id, int left, int top, int right, int bottom, LPDIRECT3DTEXTURE9 tex);
	LPSPRITE Get(int id);
	void Clear();
	static Sprites* GetInstance();
};

