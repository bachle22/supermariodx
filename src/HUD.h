#pragma once

#include "Sprites.h"
#include "Textures.h"
#include "Font.h"
#include "Animations.h"

constexpr float HUD_LARGE_X = 5;
constexpr float HUD_SMALL_X = 172;
constexpr int HUD_BAR_HEIGHT = 42;
constexpr int HUD_Y_TRANSFROM = 38;

constexpr float MARGIN = 2;
constexpr float STATS_OFFSET_X = 10;
constexpr float STAGE_X = 46;
constexpr float STATS_Y = 9;	// From HUD edge to element
constexpr float ARROW_X = 62;
constexpr float LIVES_Y = 17;
constexpr float ICON_X = 14;
constexpr float MONEY_X = 150;
constexpr float POWER_X = 111;
constexpr float TIME_X = 134;
constexpr float TIME_Y = 17;

enum HUDTextures
{
	STATS = 50001,
	POWERUP = 50002,
	ICON_MARIO = 50003,
	ARROW_FILLED = 50005,
	POWER_FILLED = 50006,
	ARROW_EMPTY = 50007,
	POWER_EMPTY = 50008,
};

enum TextLength
{
	SCORE_LENGTH = 7,
	TIME_LENGTH = 3,
	SCORE_MAX = 9999999,
	TIME_MAX = 999,
};


class HUD
{
	int offset_y;
	float x, y;

	int world, lives, powerMeter;

	Font* font;
	LPSPRITE stage;
	LPSPRITE statsHUD, powerupHUD, icon;
	LPSPRITE arrowFilled, powerFilled;
	LPSPRITE arrowEmpty, powerEmpty;
	LPDIRECT3DTEXTURE9 background;
	LPANIMATION powerMeterAnimSet;
	std::vector<LPSPRITE> score, money, time;

public:
	HUD();
	void Render();
	void SetScore(int);
	void SetMoney(int);
	void SetTime(int);
	void SetPowerMeter(int);
};

