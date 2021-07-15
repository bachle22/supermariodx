#include "Point.h"
#include "Debug.h"
#include "Mario.h"
#include "ScenePlayer.h"
#include "Game.h"
#include "Stats.h"

Point::Point(float x, float y, int type)
{
	this->x = x;
	this->y = y;
	SetState(type);
	entryY = y;
	sprite = Sprites::GetInstance()->Get(state);
	frameStart = GetTickCount64();
	tickPerFrame = 1000 / MAX_FRAME_RATE;
}

void Point::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	// Independent timer
	ULONGLONG dti = GetTickCount64() - frameStart;
	if (dti >= tickPerFrame) {
		dt = dti;
		frameStart = GetTickCount64();
	}

	GameObject::Update(dt);
	y += dy;

	// Float with easing at the end
	vy = -POINT_FLOATING_SPEED * (1 - (entryY - y) / POINT_FLOATING_HEIGHT);
	if (vy > POINT_MIN_SPEED) vy = 0;

	if (vy >= 0) Disable();
}

void Point::Render()
{
	sprite->Draw(NOFLIP, (x), y, OPAQUED);
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
	Stats* stats = Stats::GetInstance();

	switch (state)
	{
	case POINT_100:
		stats->AddPoint(SCORE_100);
		break;
	case POINT_1000:
		stats->AddPoint(SCORE_1000);
		break;
	case POINT_1UP:
		stats->AddPoint(SCORE_1UP);
		break;
	}
}