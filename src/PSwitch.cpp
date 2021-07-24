#include "PSwitch.h"
#include "Game.h"
#include "ScenePlayer.h"
#include "Brick.h"
#include "Coin.h"

PSwitch::PSwitch(float x, float y)
{
	this->x = x;
	this->y = y;
	animation_set = AnimationSets::GetInstance()->Get(PSWITCH_ANISET_ID);
	SetState(PSWITCH_OFF);
}

void PSwitch::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == PSWITCH_ON) return;
	left = x;
	top = y;
	right = x + TILE_WIDTH;
	bottom = y + TILE_HEIGHT;
}

void PSwitch::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);

}

void PSwitch::Render()
{
	animation_set->at(state)->Render(x, y);
}

void PSwitch::SetState(int state)
{
	GameObject::SetState(state);

	switch (state)
	{
	case PSWITCH_OFF:
		break;
	case PSWITCH_ON:
		y += PSWITCH_OFFSET_Y;
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		std::vector<LPGAMEOBJECT>* objects = ((ScenePlayer*)scene)->GetObjects();
		for (int i = 0; i < objects->size(); i++)
		{
			LPGAMEOBJECT obj = objects->at(i);
			if (dynamic_cast<Brick*>(obj))
			{
				Brick* brick = dynamic_cast<Brick*>(obj);
				if (brick->GetType() == BRICK_BREAKABLE)
				{
					float l, t, r, b;
					brick->GetBoundingBox(l, t, r, b);
					obj->Disable();
					Coin* coin = new Coin(l, t, COIN_LARGE_STATIC);
					((ScenePlayer*)scene)->AddObject(coin);
				}
				
			}
		}
		break;
	}
}

void PSwitch::Switch()
{
	if (state == PSWITCH_OFF) SetState(PSWITCH_ON);
}
