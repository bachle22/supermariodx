#pragma once

constexpr auto WINDOW_CLASS_NAME = L"SMBGameWindow";
constexpr auto MAIN_WINDOW_TITLE = L"Super Mario Bros 3";

constexpr int SCREEN_WIDTH = 272;
constexpr int SCREEN_HEIGHT = 272;

constexpr int SIZE_MULTIPLIER = 3;

constexpr int MAX_FRAME_RATE = 75;

#pragma warning(push, 0)
constexpr D3DCOLOR D3DCOLOR_BLACK = D3DCOLOR_XRGB(0, 0, 0);
constexpr D3DCOLOR D3DCOLOR_WHITE = D3DCOLOR_XRGB(255, 255, 255);
#pragma warning( pop )



constexpr int TILE_WIDTH = 16;
constexpr int TILE_HEIGHT = 16;

enum Direction
{
	LEFT = 0,
	RIGHT = 1,
	UP = 2,
	DOWN = 3,
};

class GameObject;
typedef GameObject* LPGAMEOBJECT;

struct CollisionEvent;
typedef CollisionEvent* LPCOLLISIONEVENT;