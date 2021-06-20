#pragma once

constexpr auto WINDOW_CLASS_NAME = L"SMBGameWindow";
constexpr auto MAIN_WINDOW_TITLE = L"Super Mario Bros 3";

constexpr int SCREEN_WIDTH = 512;
constexpr int SCREEN_HEIGHT = 480;

#define MAX_FRAME_RATE 120

#define ID_TEX_MARIO 0
#define ID_TEX_ENEMY 10
#define ID_TEX_MISC 20

constexpr D3DCOLOR D3DCOLOR_BLACK = D3DCOLOR_XRGB(0, 0, 0);
constexpr D3DCOLOR D3DCOLOR_WHITE = D3DCOLOR_XRGB(255, 255, 255);

#define ID_TEX_BBOX -100		// special texture to draw object bounding box

class GameObject;
typedef GameObject* LPGAMEOBJECT;

struct CollisionEvent;
typedef CollisionEvent* LPCOLLISIONEVENT;
