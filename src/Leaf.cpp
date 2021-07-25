#include "Leaf.h"
#include "Game.h"
#include "ScenePlayer.h"
#include "Point.h"

Leaf::Leaf(float x, float y)
{
	this->x = x;
	this->y = y;
	entryX = x;
	entryY = y;

	sprite = Sprites::GetInstance()->Get(LEAF_SPRITE_ID);
	growingDelay = 0;
	SetState(LEAF_GROWING);
}

void Leaf::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + LEAF_WIDTH;
	bottom = y + LEAF_HEIGHT;
}

void Leaf::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);
	x += dx;
	y += dy;

	if (vy > 0 && y < entryY)
	{
		SetState(LEAF_MOVING);
	}

	if (state == LEAF_MOVING)
	{
		if (x - entryX > LEAF_MAX_OFFSET) nx = -1;
		else if (entryX - x > LEAF_MAX_OFFSET) nx = 1;

		// Making leaf moving upward at the end
		vx = nx * LEAF_MOVING_SPEED * abs(abs(x - entryX) / 22 - 1) * 1.5f;

		vy -= (1 - abs(x - entryX) / LEAF_MAX_OFFSET) * LEAF_ACCELERATION_Y;
	}

	vy += GLOBAL_GRAVITY / 2 * dt;
	if (vy >= LEAF_TERMINAL_VELOCITY) vy = LEAF_TERMINAL_VELOCITY;
}

void Leaf::Render()
{
	RenderBoundingBox();
	sprite->Draw(nx, x, y);
}

void Leaf::SetState(int state)
{
	GameObject::SetState(state);

	switch (state)
	{
	case LEAF_GROWING:
	{
		vy = -LEAF_FLY_SPEED;
		float marioX, marioY;
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->GetPlayer()->GetPosition(marioX, marioY);

		if (entryX > marioX) nx = 1;
		else nx = -1;
		break;
	}
	case LEAF_MOVING:
		vx = nx * LEAF_MOVING_SPEED;
		priority = PRIORITY_HIGH;
		break;
	}
}

void Leaf::Consume()
{
	Point* point = new Point(x, y, POINT_1000);
	LPSCENE scene = Game::GetInstance()->GetCurrentScene();
	((ScenePlayer*)scene)->AddObject(point);
	this->Disable();
}