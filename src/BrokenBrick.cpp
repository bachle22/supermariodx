#include "BrokenBrick.h"

BrokenBrick::BrokenBrick(float x, float y)
{
	this->x = x;
	this->y = y;
	frameStart = GetTickCount64();
	tickPerFrame = 1000 / MAX_FRAME_RATE;
	timer = 0;
	animation_set = AnimationSets::GetInstance()->Get(BROKEN_BRICK_ANI_ID);
	isStarted = false;
	priority = PRIORITY_HIGH;
}

void BrokenBrick::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	// Independent timer
	ULONGLONG dti = GetTickCount64() - frameStart;
	if (dti >= tickPerFrame) {
		dt = dti;
		frameStart = GetTickCount64();
	}

	timer += dt;
	if (timer > BROKEN_BRICK_TIMEOUT) Disable();

	GameObject::Update(dt);
	x += dx;
	y += dy;
	vy += GLOBAL_GRAVITY * dt;
	if (vy > GLOBAL_TERMINAL_VELOCITY) vy = GLOBAL_TERMINAL_VELOCITY;

	if (!isStarted)
	{
		vy -= BROKEN_BRICK_ACCELERATION;
		isStarted = true;
	}

}

void BrokenBrick::Render()
{
	int offsetX, offsetY;
	offsetX = (int) timer / BROKEN_BRICK_OFFSET_X;
	offsetY = (int) timer / BROKEN_BRICK_OFFSET_Y;
	animation_set->at(0)->Render(-1, x - offsetX, y + offsetY);
	animation_set->at(0)->Render(1, x + BROKEN_BRICK_WIDTH + offsetX, y + offsetY);
	animation_set->at(0)->Render(-1, x - offsetX, y);
	animation_set->at(0)->Render(1, x + BROKEN_BRICK_WIDTH + offsetX, y);
}

void BrokenBrick::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	return;
}

void BrokenBrick::SetState(int state)
{
	return;
}