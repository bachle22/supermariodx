#include "FloatingWood.h"
#include "Mario.h"
#include "ScenePlayer.h"
#include "Game.h"

FloatingWood::FloatingWood()
{
	SetState(FLOATING_WOOD_MOVING);
	isTouched = false;
	priority = PRIORITY_HIGH;
}

void FloatingWood::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + FLOATING_WOOD_WIDTH;
	bottom = y + FLOATING_WOOD_HEIGHT;
}

void FloatingWood::Render()
{
	animation_set->at(0)->Render(x, y);
	RenderBoundingBox();
}


void FloatingWood::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);

	x += dx;
	y += dy;

	if (isTouched)
	{
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		Mario* mario = ((ScenePlayer*)scene)->GetPlayer();
		if (mario->GetAction(TOUCHING_WOOD))
		{
			float mx, my, mvx, mvy;
			mario->GetPosition(mx, my);
			mario->SetPosition(mx, y - mario->GetHeight() - .1f);
			mario->GetSpeed(mvx, mvy);
			mario->SetSpeed(mvx, vy);
		}
		else isTouched = false;
	}

	if (vy > GLOBAL_TERMINAL_VELOCITY) vy = GLOBAL_TERMINAL_VELOCITY;
	else if (isTouched || vy != 0) vy += FLOATING_WOOD_GRAVITY;

}

void FloatingWood::SetState(int state)
{
	GameObject::SetState(state);

	switch (state)
	{
	case FLOATING_WOOD_MOVING:
		vx = -FLOATING_WOOD_SPEED;
		vy = 0;
		break;
	case FLOATING_WOOD_FALLING:
		vx = 0;
		isTouched = true;
		break;
	default:
		break;
	}
}