#pragma once

constexpr auto WINDOW_CLASS_NAME = L"SMBGameWindow";
constexpr auto MAIN_WINDOW_TITLE = L"Super Mario Bros 3";

constexpr int SCREEN_WIDTH = 256;
constexpr int SCREEN_HEIGHT = 240;

constexpr int MAX_FRAME_RATE = 120;

constexpr D3DCOLOR D3DCOLOR_BLACK = D3DCOLOR_XRGB(0, 0, 0);
constexpr D3DCOLOR D3DCOLOR_WHITE = D3DCOLOR_XRGB(255, 255, 255);

#define ID_TEX_BBOX -100		// special texture to draw object bounding box

class GameObject;
typedef GameObject* LPGAMEOBJECT;

struct CollisionEvent;
typedef CollisionEvent* LPCOLLISIONEVENT;
