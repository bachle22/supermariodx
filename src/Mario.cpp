#include <vector>

#include "Mario.h"
#include "Goomba.h"
#include "Debug.h"
#include "Definition.h"
#include "Collision.h"
#include "Portal.h"
#include "Game.h"
#include "Types.h"

Mario::Mario(float x, float y) : GameObject()
{
	level = MARIO_LEVEL_SMALL;
	untouchable = 0;
	SetState(MARIO_STATE_IDLE);
	ax = 0; ay = 0;
	start_x = x;
	start_y = y;
	this->x = x;
	this->y = y;

	canJumpAgain = true;
}

void Mario::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	// Calculate dx, dy 
	GameObject::Update(dt);
	Movement();


	std::vector<LPCOLLISIONEVENT> coEvents;
	std::vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != MARIO_STATE_DIE)
		CalculatePotentialCollisions(coObjects, coEvents);

	// reset untouchable timer if untouchable time has passed
	if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	// No collision occured, proceed normally
	if (coEvents.size() == 0)
	{
		x += dx;
		y += dy;

	}
	else
	{
		float min_tx, min_ty, nx = 0, ny;
		float rdx = 0;
		float rdy = 0;

		// TODO: Improve this function
		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		//if (rdx != 0 && rdx!=dx)
		//	x += nx*abs(rdx); 

		// block 
		x += min_tx * dx + nx * 0.4f;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
		y += min_ty * dy + ny * 0.4f;

		// if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		// Stop when touching edge
		if (min_ty == 1 && !isJumping) {
			ax = 0;
			edges[RIGHT] = true;
		}

		// Reset jump when touching the ground
		if (ny == -1) IsJumping(false);

		// DebugOut(L"min_tx %f min_ty %f nx %f ny %f rdx %f rdy %f\n", min_tx, min_ty, nx, ny, rdx, rdy);

		//
		// Collision logic with other objects
		//
		for (size_t i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<Goomba*>(e->obj)) // if e->obj is Goomba 
			{
				Goomba* goomba = dynamic_cast<Goomba*>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					if (goomba->GetState() != GOOMBA_STATE_DIE)
					{
						goomba->SetState(GOOMBA_STATE_DIE);
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
				}
				else if (e->nx != 0)
				{
					if (untouchable == 0)
					{
						if (goomba->GetState() != GOOMBA_STATE_DIE)
						{
							if (level > MARIO_LEVEL_SMALL)
							{
								level = MARIO_LEVEL_SMALL;
								StartUntouchable();
							}
							else
								SetState(MARIO_STATE_DIE);
						}
					}
				}
			} // if Goomba
			else if (dynamic_cast<Portal*>(e->obj))
			{
				Portal* p = dynamic_cast<Portal*>(e->obj);
				Game::GetInstance()->SwitchScene(p->GetSceneId());
			}
		}
	}
	// clean up collision events
	for (size_t i = 0; i < coEvents.size(); i++) delete coEvents[i];
	//DebugOut(L"x %f y %f vx %f vy %f dx %f dy %f\n", x, y, vx, vy, dx, dy);
}

void Mario::Render()
{
	int ani = NULL;
	if (state == MARIO_STATE_DIE)
		ani = DIE;
	else
		switch (level) {
		case MARIO_LEVEL_BIG:
			if (vx != 0 && ax != 0)
			{
				// Stop animating when hit an edge
				if (!edges[RIGHT]) ani = BIG_WALKING;
				else ani = BIG_IDLE;

				// Efficently check if ax * nx < 0
				if ((AS_INT(ax) ^ nx) < 0) ani = BIG_BRAKING;
			}
			else ani = BIG_IDLE;
			if (isJumping) {
				if (vy < 0) ani = BIG_JUMPING;
				else ani = BIG_JUMPED;
			}
			if (!movement[LEFT] && !movement[RIGHT] && movement[DOWN]) ani = BIG_DUCKING;
			break;
		case MARIO_LEVEL_SMALL:
			if (AS_INT(vx) != 0 && AS_INT(ax) != 0)
			{
				if (!edges[RIGHT]) ani = SMALL_WALKING;
				else ani = SMALL_IDLE;
				if ((AS_INT(ax) ^ nx) < 0) ani = SMALL_BRAKING;
			}
			else ani = SMALL_IDLE;
			if (isJumping) ani = SMALL_JUMPING;
			break;
		}

	int alpha = 255;
	if (untouchable) alpha = 128;

	animation_set->at(ani)->Render(nx, x, y, alpha);

	RenderBoundingBox();
}

void Mario::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case MARIO_STATE_DIE:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	}
}

void Mario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;

	if (level == MARIO_LEVEL_BIG)
	{
		right = x + MARIO_BIG_BBOX_WIDTH;
		bottom = y + MARIO_BIG_BBOX_HEIGHT;
	}
	else
	{
		right = x + MARIO_SMALL_BBOX_WIDTH;
		bottom = y + MARIO_SMALL_BBOX_HEIGHT;
	}
}

/*
	Reset Mario status to the beginning state of a scene
*/
void Mario::Reset()
{
	SetState(MARIO_STATE_IDLE);
	SetLevel(MARIO_LEVEL_BIG);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
}

void Mario::Movement()
{
	// Velocity with friction
	vx = level == MARIO_LEVEL_SMALL ? MARIO_WALKING_SPEED_SMALL * ax : MARIO_WALKING_SPEED * ax;
	// Simple fall down
	vy += MARIO_GRAVITY * dt;

	if (movement[LEFT])
	{
		//SetState(MARIO_STATE_WALKING_LEFT);
		nx = -1;
		if (ax > -1) ax -= MARIO_ACCELERATION_X;
		if (vx > 0) ax -= MARIO_ACCELERATION_X;

		edges[RIGHT] = false;
	}
	else if (movement[RIGHT])
	{
		//SetState(MARIO_STATE_WALKING_RIGHT);
		nx = 1;
		if (ax < 1) ax += MARIO_ACCELERATION_X;
		if (vx <= 0) ax += MARIO_ACCELERATION_X;

	}

	if (movement[UP] && canJumpAgain)
	{
		if (isTurbo) {
			//SetState(MARIO_STATE_JUMP_HIGH);
			if (!isJumping)
			{
				last_y = y;
				vy = -MARIO_JUMP_SPEED_HIGH;
			}
			if (last_y - y < MARIO_JUMP_HEIGHT_MAX) {
				vy -= 0.001f;
				// Gravity compensation
				vy -= MARIO_GRAVITY * dt;
			}
			else canJumpAgain = false;

		}
		else
		{
			//SetState(MARIO_STATE_JUMP_LOW);
			if (!isJumping) vy = -MARIO_JUMP_SPEED_LOW;
		}
		IsJumping(true);
		edges[RIGHT] = false;
	}

	if (movement[DOWN])
	{
		if (level != MARIO_LEVEL_SMALL && !movement[LEFT] && !movement[RIGHT]) {
			if (ax != 0) ax += ax > 0 ? -MARIO_ACCELERATION_X : MARIO_ACCELERATION_X;
		}
	}



	// Quickly check if UP/DOWN/LEFT/RIGHT all equal zero
	//if (!*(int*)movement) {
	//	SetState(MARIO_STATE_IDLE);
	//	if (ax < 0) ax += MARIO_ACCELERATION_X;
	//	if (ax > 0) ax -= MARIO_ACCELERATION_X;
	//}

	if (abs(ax) <= MARIO_INERTIA) ax = 0;
	if (ax < 0 && !movement[LEFT])
		ax += level == MARIO_LEVEL_SMALL ? MARIO_INERTIA_SMALL : MARIO_INERTIA;
	if (ax > 0 && !movement[RIGHT])
		ax -= level == MARIO_LEVEL_SMALL ? MARIO_INERTIA_SMALL : MARIO_INERTIA;

}