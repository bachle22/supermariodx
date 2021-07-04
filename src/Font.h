#pragma once

#include <unordered_map>
#include "Sprites.h"

constexpr float FONT_SIZE = 8.01f;

enum FontSprite {
	FONT_0 = 50013,
	FONT_1 = 50014,
	FONT_2 = 50015,
	FONT_3 = 50016,
	FONT_4 = 50017,
	FONT_5 = 50018,
	FONT_6 = 50019,
	FONT_7 = 50020,
	FONT_8 = 50021,
	FONT_9 = 50022,
	FONT_A = 50023,
	FONT_B = 50024,
	FONT_C = 50025,
	FONT_D = 50026,
	FONT_E = 50027,
	FONT_F = 50028,
	FONT_G = 50029,
	FONT_H = 50030,
	FONT_I = 50031,
	FONT_J = 50032,
	FONT_K = 50033,
	FONT_L = 50034,
	FONT_M = 50035,
	FONT_N = 50036,
	FONT_O = 50037,
	FONT_P = 50038,
	FONT_Q = 50039,
	FONT_R = 50040,
	FONT_S = 50041,
	FONT_T = 50042,
	FONT_U = 50043,
	FONT_V = 50044,
	FONT_W = 50045,
	FONT_X = 50046,
	FONT_Y = 50047,
	FONT_Z = 50048,
};

class Font
{
	std::unordered_map<char, LPSPRITE> fonts;

public:
	Font();
	LPSPRITE Map(char c);
	std::vector<LPSPRITE> StringToSprites(std::string str);
};

