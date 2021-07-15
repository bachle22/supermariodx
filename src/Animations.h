#pragma once

#include <unordered_map>
#include "Sprites.h"

/*
	Sprite animation
*/
class Frame
{
	LPSPRITE sprite;
	ULONGLONG time;

public:
	Frame(LPSPRITE sprite, ULONGLONG time) { this->sprite = sprite; this->time = time; }
	ULONGLONG GetTime() { return time; }
	LPSPRITE GetSprite() { return sprite; }
};

typedef Frame* LPANIMATION_FRAME;

class Animation
{
	ULONGLONG lastFrameTime;
	int currentFrame;
	std::vector<LPANIMATION_FRAME> frames;
public:
	Animation() { lastFrameTime = -1; currentFrame = -1; }
	void Add(int spriteId, ULONGLONG time = 0);
	void Render(int nx, float x, float y, int alpha, D3DXVECTOR2 translation);
	void Render(int nx, float x, float y, int alpha = 255);
	void Render(float x, float y, int alpha = 255);
};

typedef Animation* LPANIMATION;

class Animations
{
	static Animations* __instance;

	std::unordered_map<int, LPANIMATION> animations;

public:
	void Add(int id, LPANIMATION ani);
	LPANIMATION Get(int id);
	void Clear();

	static Animations* GetInstance();
};
