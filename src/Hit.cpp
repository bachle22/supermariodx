#include "Hit.h"

Hit::Hit(float x, float y)
{
	this->x = x;
	this->y = y;
	animation_set = AnimationSets::GetInstance()->Get(HIT_ANIMSET_ID);
	frameStart = GetTickCount64();
	tickPerFrame = 1000 / MAX_FRAME_RATE;
	timer = 0;
	priority = PRIORITY_HIGH;
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
	animation_set->at(0)->Render(x, y);
}

void Hit::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	return;
}

void Hit::SetState(int state)
{
	return;
}