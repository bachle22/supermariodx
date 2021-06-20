#pragma once

#include <unordered_map>
#include "Sprites.h"

/*
	Sprite animation
*/
class Frame
{
	LPSPRITE sprite;
	DWORD time;

public:
	Frame(LPSPRITE sprite, int time) { this->sprite = sprite; this->time = time; }
	DWORD GetTime() { return time; }
	LPSPRITE GetSprite() { return sprite; }
};

typedef Frame* LPANIMATION_FRAME;

class Animation
{
	DWORD lastFrameTime;
	int defaultTime;
	int currentFrame;
	std::vector<LPANIMATION_FRAME> frames;
public:
	Animation(int defaultTime) { this->defaultTime = defaultTime; lastFrameTime = -1; currentFrame = -1; }
	void Add(int spriteId, DWORD time = 0);
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
