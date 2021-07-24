#include "Warp.h"

Warp::Warp(float x, float y)
{
	this->x = x;
	this->y = y;
	animation_set = AnimationSets::GetInstance()->Get(WARP_ANI_ID);
	frameStart = GetTickCount64();
	tickPerFrame = 1000 / MAX_FRAME_RATE;
	timer = 0;
}

void Warp::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	// Independent timer
	ULONGLONG dti = GetTickCount64() - frameStart;
	if (dti >= tickPerFrame) {
		dt = dti;
		frameStart = GetTickCount64();
	}

	timer += dt;
	if (timer >= WARP_TIMEOUT) this->Disable();
	GameObject::Update(dt);

}

void Warp::Render()
{
	animation_set->at(0)->Render(x, y);
}

void Warp::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	return;
}

void Warp::SetState(int state)
{
	return;
}