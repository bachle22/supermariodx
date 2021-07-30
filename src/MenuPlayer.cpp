#include "MenuPlayer.h"
#include "Game.h"
#include "MenuObject.h"
#include "SceneMenu.h"
#include "Debug.h"

MenuPlayer::MenuPlayer()
{
	//level = CBackUp::GetInstance()->level;
	//SetState(MARIO_STATE_IDLE);
	//start_x = x;
	//start_y = y;

	//SetMove(false, false, true, false);
}

void MenuPlayer::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + TILE_WIDTH;
	bottom = y + TILE_HEIGHT;
}

void MenuPlayer::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt);
	x += dx;
	y += dy;


	for (size_t i = 0; i < coObjects->size(); i++)
	{
		LPGAMEOBJECT obj = CheckOverlap(coObjects->at(i));
		if (dynamic_cast<MenuObject*>(obj))
		{
			MenuObject* m = dynamic_cast<MenuObject*>(obj);
			if (m->GetType() == OBJECT_TYPE_PANEL || m->GetType() == OBJECT_TYPE_STOP)
			{
				float mx, my;
				m->GetPosition(mx, my);
				if (abs(x - lastX) >= 16 || abs(y - lastY) >= 16)
				{
					if (abs(x - mx) < 6 && abs(y - my) < 6) {
						x = mx;
						y = my;
						SetState(PLAYER_STATE_IDLE);
					}
					lastX = x;
					lastY = y;
				}

				SetMovement(m->GetMovement());

				if (m->GetType() == OBJECT_TYPE_PANEL) sceneId = m->GetSceneId();
				else sceneId = 0;
			}
		}
	}
}
void MenuPlayer::Render()
{
	animation_set->at(0)->Render(x, y);
	//animation_set->at(level - 1)->Render(x, y);
	RenderBoundingBox();
}
void MenuPlayer::SetState(int state)
{
	GameObject::SetState(state);
	
	switch (state)
	{
	case PLAYER_STATE_IDLE:
		vx = vy = 0;
		break;
	case PLAYER_STATE_LEFT:
		if (GetMovement(LEFT))
		{
			vx = -PLAYER_SPEED;
			vy = 0;
		}
		break;
	case PLAYER_STATE_RIGHT:
		if (GetMovement(RIGHT))
		{
			vx = PLAYER_SPEED;
			vy = 0;
		}
		break;
	case PLAYER_STATE_UP:
		if (GetMovement(UP))
		{
			vx = 0;
			vy = -PLAYER_SPEED;
		}
		break;
	case PLAYER_STATE_DOWN:
		if (GetMovement(DOWN))
		{
			vx = 0;
			vy = PLAYER_SPEED;
		}
		break;
	}
}

void MenuPlayer::SetMovement(bool* movement)
{
	this->movement[LEFT] = movement[LEFT];
	this->movement[RIGHT] = movement[RIGHT];
	this->movement[UP] = movement[UP];
	this->movement[DOWN] = movement[DOWN];
}

void MenuPlayer::SetPosition(float x, float y)
{
	GameObject::SetPosition(x, y);
	lastX = x;
	lastY = y;
}

void MenuPlayer::SwitchScene()
{
	if (sceneId <= 0) return;
	Game::GetInstance()->SwitchScene(sceneId);
}

void MenuPlayer::UnsetMovement()
{
	this->movement[LEFT] = false;
	this->movement[RIGHT] = false;
	this->movement[UP] = false;
	this->movement[DOWN] = false;
}