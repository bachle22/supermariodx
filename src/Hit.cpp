#include "Hit.h"
#include "Debug.h"
#include "Mario.h"
#include "ScenePlayer.h"
#include "Game.h"
#include "Stats.h"

Hit::Hit(float x, float y)
{
	this->x = x;
	this->y = y;
	ani = AnimationSets::GetInstance()->Get(HIT_ANIMSET_ID);
	frameStart = GetTickCount64();
	tickPerFrame = 1000 / MAX_FRAME_RATE;
	timer = 0;
}

void Hit::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	// Independent timer
	ULONGLONG dti = GetTickCount64() - frameStart;
	if (dti >= tickPerFrame) {
		dt = dti;
		frameStart = GetTickCount64();
	}

	timer += dt;
	if (timer >= HIT_ANIMATION_DURATION) this->Disable();

	GameObject::Update(dt);
	y += dy;

}

void Hit::Render()
{
	ani->at(0)->Render(x, y);
}

void Hit::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	return;
}

void Hit::SetState(int state)
{
	GameObject::SetState(state);
}