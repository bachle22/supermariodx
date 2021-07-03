#pragma once

#include "Sprites.h"
#include "Textures.h"
#include "Font.h"

constexpr int HUD_LARGE_X = 5;
constexpr float HUD_SMALL_X = 172.1f;
constexpr int HUD_BAR_HEIGHT = 42;
constexpr int HUD_Y_TRANSFROM = 38;

enum HUDTextures
{
	STATS = 50001,
	POWERUP = 50002,
	ICON_MARIO = 50003,
	ARROW_EMPTY = 50007,
	POWER_EMPTY = 50008,

	MARGIN = 2,
	STATS_OFFSET_X = 10,
	STAGE_X = 46,
	STATS_Y = 9,	// From HUD edge to element
	ARROW_X = 62,
	LIVES_Y = 17,
	ICON_X = 14,
	MONEY_X = 150,
	POWER_X = 111,
	TIME_X = 134,
	TIME_Y = 17
};

enum TextLength
{
	SCORE_LENGTH = 7,
	TIME_LENGTH = 3,
};

class HUD
{
	int offset_y;
	float x, y;

	int world, lives;

	Font* font;

	LPSPRITE stage;
	LPSPRITE statsHUD, powerupHUD, icon;
	LPSPRITE arrowEmpty, powerEmpty;
	std::vector<LPSPRITE> score, money, time;
	LPDIRECT3DTEXTURE9 background;

public:
	HUD();
	void Render();
	void SetScore(int);
	void SetMoney(int);
	void SetTime(int);
};

