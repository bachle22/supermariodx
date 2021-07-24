#include <math.h> 

#include "Plant.h"
#include "Game.h"
#include "ScenePlayer.h"
#include "Point.h"
#include "Platform.h"
#include "Projectile.h"
#include "Warp.h"
#include "Point.h"

Plant::Plant(int type)
{
	timer = 0;
	this->type = type;
	SetState(PLANT_STATE_HIDING);
	if (type == PLANT_RED_SHOOTER) height = PLANT_RED_HEIGHT;
	else height = PLANT_GREEN_HEIGHT;

}

void Plant::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (state == PLANT_STATE_HIDING) return;
	left = x;
	top = y;
	right = x + PLANT_WIDTH;
	bottom = y + height;
}

void Plant::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);
	y += dy;

	timer += dt;

	float marioX, marioY;
	LPSCENE scene = Game::GetInstance()->GetCurrentScene();
	((ScenePlayer*)scene)->GetPlayer()->GetPosition(marioX, marioY);
	if (marioX < x) nx = -1;
	else nx = 1;
	if (marioY < y) ny = -1;
	else ny = 1;

	if (type == PLANT_GREEN_SHOOTER || type == PLANT_RED_SHOOTER)
	{
		if (state == PLANT_STATE_HIDING)
		{
			if (timer >= PLANT_SHOOTER_HIDING_INTERVAL)
			{
				SetState(PLANT_STATE_EXPANDING);
				timer = 0;
			}
		}
		else
			if (state == PLANT_STATE_ATTACKING)
			{
				if (!isProjectileShooted)
				{
					if (timer >= PLANT_PROJECTILE_DELAY)
					{
						int direction;
						// The principal value of the arc tangent of y/x, expressed in PI radian.
						float piRad = atan2f(marioY - y, marioX - x) / -D3DX_PI;

						// Find direction to shoot projectile
						if (piRad >= 0)
						{
							if (piRad >= 0 && piRad < PROJECTILE_22DEG)
								direction = UPPER_RIGHT;
							if (piRad >= PROJECTILE_22DEG && piRad < PROJECTILE_90DEG)
								direction = TOP_RIGHT;
							if (piRad >= PROJECTILE_90DEG && piRad < PROJECTILE_157DEG)
								direction = TOP_LEFT;
							if (piRad >= PROJECTILE_157DEG)
								direction = UPPER_LEFT;
						}
						else
						{
							if (piRad >= -PROJECTILE_22DEG && piRad < 0)
								direction = LOWER_RIGHT;
							if (piRad >= -PROJECTILE_90DEG && piRad < -PROJECTILE_22DEG)
								direction = BOTTOM_RIGHT;
							if (piRad >= -PROJECTILE_157DEG && piRad < -PROJECTILE_90DEG)
								direction = BOTTOM_LEFT;
							if (piRad < -PROJECTILE_157DEG)
								direction = LOWER_LEFT;
						}


						float projectileX = nx > 0 ?
							x + PLANT_PROJECTILE_OFFSET_XX :
							x - PLANT_PROJECTILE_OFFSET_XY;
						float projectileY = ny > 0 ?
							y + PLANT_PROJECTILE_OFFSET_YX :
							y - PLANT_PROJECTILE_OFFSET_YY;
						Projectile* projectile = new Projectile(projectileX, projectileY, direction);
						LPSCENE scene = Game::GetInstance()->GetCurrentScene();
						((ScenePlayer*)scene)->AddObject(projectile);
						isProjectileShooted = true;
					}
				}

				if (timer >= PLANT_SHOOTER_ATTACKING_INTERVAL)
				{
					SetState(PLANT_STATE_COLLAPSING);
					timer = 0;
				}

			}
	}
	else
	{
		if (state == PLANT_STATE_HIDING)
		{
			if (timer >= PLANT_BITER_HIDING_INTERVAL)
			{
				SetState(PLANT_STATE_EXPANDING);
				timer = 0;
			}
		}
		else if (state == PLANT_STATE_ATTACKING)
		{
			if (timer >= PLANT_BITER_ATTACKING_INTERVAL)
			{
				SetState(PLANT_STATE_COLLAPSING);
				timer = 0;
			}
		}
	}

	if (y <= entryY && state == PLANT_STATE_EXPANDING)
	{
		vy = 0;
		y = entryY;
		SetState(PLANT_STATE_ATTACKING);
		timer = 0;
	}
	else if (y >= entryY + height && state == PLANT_STATE_COLLAPSING)
	{
		vy = 0;
		SetState(PLANT_STATE_HIDING);
		timer = 0;
	}
	else if (state == PLANT_STATE_HIDING)
	{
		Mario* mario = ((ScenePlayer*)Game::GetInstance()->GetCurrentScene())->GetPlayer();
		if (mario == NULL) return;
		float ml, mt, mr, mb;
		mario->GetBoundingBox(ml, mt, mr, mb);
		// Stop the plant from expanding when Mario is nearby
		if (mr + 9 > x && ml < x + 25 && mb > entryY)
			timer = 0;
	}

}

void Plant::Render()
{
	int ani;
	switch (type)
	{
	case PLANT_GREEN_BITER:
	{
		ani = PLANT_GREEN_ANI;
		break;
	}
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
	case PLANT_GREEN_SHOOTER:
	{
		if (ny > 0) ani = PLANT_GREEN_ANI_BITING_DOWN;
		else ani = PLANT_GREEN_ANI_BITING_UP;

		if (state == PLANT_STATE_ATTACKING)
		{
			if (ny > 0)
				ani = PLANT_GREEN_ANI_SHOOTING_UP;
			else ani = PLANT_GREEN_ANI_SHOOTING_DOWN;
		}
		break;
	}
	default:
		ani = 0;
	}

	animation_set->at(ani)->Render(nx, x, y, PLANT_WIDTH, (int)(entryY + height - y));
	RenderBoundingBox();
}

void Plant::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case PLANT_STATE_EXPANDING:
		vy -= PLANT_VELOCITY_Y;
		break;
	case PLANT_STATE_COLLAPSING:
		vy += PLANT_VELOCITY_Y;
	case PLANT_STATE_ATTACKING:
		isProjectileShooted = false;
		break;
	case PLANT_STATE_DEAD:
		Warp* warp = new Warp(x, y + PLANT_WARP_EFFECT_Y);
		Point* point = new Point(x, y, POINT_100);

		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->AddObject(warp);
		((ScenePlayer*)scene)->AddObject(point);
		this->Disable();
		break;
	}
}

void Plant::SetPosition(float x, float y)
{
	entryY = y;
	y += height;
	GameObject::SetPosition(x, y);
}

bool Plant::Hit()
{
	if (state != PLANT_STATE_DEAD)
	{
		SetState(PLANT_STATE_DEAD);
		return true;
	}
	else return false;
}