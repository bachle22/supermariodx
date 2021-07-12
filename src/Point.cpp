#include "Point.h"
#include "Debug.h"
#include "Mario.h"
#include "ScenePlayer.h"
#include "Game.h"

Point::Point(float x, float y, int type)
{
	this->x = x;
	this->y = y;
	SetState(type);
	entryY = y;
}

void Point::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt);
	y += dy;

	// Float with easing at the end
	vy = -POINT_FLOATING_SPEED * (1 - (entryY - y) / POINT_FLOATING_HEIGHT);
	if (vy > POINT_MIN_SPEED) vy = 0;

	if (vy >= 0) Disable();
}

void Point::Render()
{
	point = Sprites::GetInstance()->Get(state);
	point->Draw(NOFLIP, (x), y, OPAQUED);
	// RenderBoundingBox();
}

void Point::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	return;
}

void Point::SetState(int state)
{
	GameObject::SetState(state);

	LPSCENE scene = Game::GetInstance()->GetCurrentScene();
	Mario* mario = ((ScenePlayer*)scene)->GetPlayer();

	switch (state)
	{
	case POINT_100:
		
		break;
	}
}