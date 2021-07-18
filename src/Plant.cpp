#include "Plant.h"
#include "Game.h"
#include "ScenePlayer.h"
#include "Point.h"
#include "Platform.h"

Plant::Plant(int type)
{
	timer = 0;
	this->type = type;
	SetState(PLANT_STATE_HIDING);
	if (type == PLANT_RED_SHOOTER) height = PLANT_RED_BBOX_HEIGHT;
}

void Plant::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == PLANT_STATE_HIDING) return;
	left = x;
	top = y;
	right = x + PLANT_BBOX_WIDTH;
	bottom = y + height;
}

void Plant::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);
	y += dy;

	timer += dt;

	switch (type)
	{
	case PLANT_RED_SHOOTER:
	{
		if (timer >= 1500 && state == PLANT_STATE_HIDING)
		{
			SetState(PLANT_STATE_EXPANDING);
			timer = 0;
		}
		if (timer >= 1500 && state == PLANT_STATE_ATTACKING)
		{
			SetState(PLANT_STATE_COLLAPSING);
			timer = 0;
		}
		break;
	}
	}

	if (y <= entryY && state == PLANT_STATE_EXPANDING)
	{
		vy = 0;
		y = entryY;
		SetState(PLANT_STATE_ATTACKING);
		timer = 0;
	}
	if (y >= entryY + height && state == PLANT_STATE_COLLAPSING)
	{
		vy = 0;
		SetState(PLANT_STATE_HIDING);
		timer = 0;
	}

	float marioX, marioY;
	LPSCENE scene = Game::GetInstance()->GetCurrentScene();
	((ScenePlayer*)scene)->GetPlayer()->GetPosition(marioX, marioY);
	if (marioX < x) nx = -1;
	else nx = 1;
	if (marioY < y) ny = -1;
	else ny = 1;
}

void Plant::Render()
{
	int ani;
	switch (type)
	{
	case PLANT_RED_SHOOTER:
	{
		if (ny > 0) ani = PLANT_RED_ANI_BITING_DOWN;
		else ani = PLANT_RED_ANI_BITING_UP;

		if (state == PLANT_STATE_ATTACKING)
		{
			if (ny > 0)
			ani = PLANT_RED_ANI_SHOOTING_UP;
			else ani = PLANT_RED_ANI_SHOOTING_DOWN;
		}
		break;
	}
	default:
		ani = 0;
	}

	animation_set->at(ani)->Render(nx, x, y, PLANT_BBOX_WIDTH, entryY + PLANT_RED_BBOX_HEIGHT - y);
	RenderBoundingBox();
}

void Plant::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case PLANT_STATE_EXPANDING:
		vy -= 0.045f;
		break;
	case PLANT_STATE_COLLAPSING:
		vy += 0.045f;
		break;
	}
}

void Plant::SetPosition(float x, float y)
{
	entryY = y;
	y += PLANT_RED_BBOX_HEIGHT;
	GameObject::SetPosition(x, y);
}