#pragma once

#include "Sprites.h"
#include "Textures.h"

constexpr float HUD_LARGE_X = 5;
constexpr float HUD_SMALL_X = 172.1f;
constexpr int HUD_BAR_HEIGHT = 42;
constexpr int HUD_Y_TRANSFROM = 38;

enum HUDTextures
{
	STATS = 50001,
	POWERUP = 50002
};

class HUD
{
	int offset_y;
	float x, y;

	LPSPRITE statsHUD, powerupHUD;
	LPDIRECT3DTEXTURE9 background;

public:
	HUD();
	void Render();
};

