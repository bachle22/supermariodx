#pragma once

#include "Sprites.h"
#include "Textures.h"

constexpr int TRANSITION_STEP = 10;

enum TranstionType
{
	TRANSITION_UNDEFINED = -1,
	FADE_IN = 0,
	FADE_OUT = 1,
};

class Transition
{
	static Transition* __instance;

	float x, y;
	int type{};
	int opacity{};
	bool isFinished;
	LPDIRECT3DTEXTURE9 background;
public:
	Transition();
	static Transition* GetInstance();
	void Render();
	void FadeIn();
	void FadeOut();
	bool IsFinished() { return isFinished; }
};

