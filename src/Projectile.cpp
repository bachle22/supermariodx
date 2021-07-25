#include "Game.h"
#include "ScenePlayer.h"
#include "Projectile.h"
#include "Point.h"

Projectile::Projectile(float x, float y, int direction)
{
	this->x = x;
	this->y = y;
	SetState(direction);
	animation_set = AnimationSets::GetInstance()->Get(PROJECTILE_ANI_SET_ID);
	priority = PRIORITY_HIGH;
}

void Projectile::Render()
{
	animation_set->at(0)->Render(NOFLIP, x, (y));
	//RenderBoundingBox();
}

void Projectile::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + PROJECTILE_BBOX_WIDTH;
	bottom = y + PROJECTILE_BBOX_HEIGH;
}

void Projectile::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);

	x += dx;
	y += dy;
}

void Projectile::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case UPPER_RIGHT:
		vx = 0.058f;
		vy = -0.016f;
		break;
	case TOP_RIGHT:
		vx = 0.04f;
		vy = -0.04f;
		break;
	case TOP_LEFT:
		vx = -0.04f;
		vy = -0.04f;
		break;
	case UPPER_LEFT:
		vx = -0.058f;
		vy = -0.016f;
		break;
	case LOWER_RIGHT:
		vx = 0.058f;
		vy = 0.016f;
		break;
	case BOTTOM_RIGHT:
		vx = 0.04f;
		vy = 0.04f;
		break;
	case BOTTOM_LEFT:
		vx = -0.04f;
		vy = 0.04f;
		break;
	case LOWER_LEFT:
		vx = -0.058f;
		vy = 0.016f;
		break;
	}
}