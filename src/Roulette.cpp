#include "Roulette.h"

Roulette::Roulette()
{
	timer = 0;
	SetState(ROULETTE_STATE_SPINNING);
}

void Roulette::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + TILE_WIDTH;
	bottom = y + TILE_HEIGHT;
}

void Roulette::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);
	y += dy;

	if (state != ROULETTE_STATE_WITHDRAWED)
	{
		timer += dt;
		if (timer >= 100)
		{
			if (type >= 3) type = 1;
			else type++;
			timer = 0;
		}
	}

}

void Roulette::Render()
{
	int ani = ROULETTE_UNKNOWN;
	if (state == ROULETTE_STATE_WITHDRAWED) ani = type;
	animation_set->at(ani)->Render(x, y);
	RenderBoundingBox();
}

void Roulette::SetState(int state)
{
	GameObject::SetState(state);

	switch (state)
	{
	case ROULETTE_STATE_SPINNING:
		type = ROULETTE_UNKNOWN;
		break;
	case ROULETTE_STATE_WITHDRAWED:
		vy -= 0.1f;
		break;
	}
}

void Roulette::Withdraw()
{
	if (state != ROULETTE_STATE_WITHDRAWED)
		SetState(ROULETTE_STATE_WITHDRAWED);
}