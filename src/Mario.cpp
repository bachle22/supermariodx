#include <vector>

#include "Mario.h"
#include "Goomba.h"
#include "Debug.h"
#include "Definition.h"
#include "Collision.h"
#include "Portal.h"
#include "Game.h"
#include "Types.h"
#include "Platform.h"
#include "Brick.h"
#include "Mushroom.h"
#include "Block.h"
#include "ScenePlayer.h"

Mario::Mario(float x, float y) : GameObject()
{
	state = MARIO_SMALL;
	untouchable = 0;
	ax = 0; ay = 0;
	start_x = x;
	start_y = y;
	this->x = x;
	this->y = y;

	powerMeter = 0;
	powerTimer = 0;
	flyTimer = 0;

	lastTimeGainPower = 0;
	lastTimeDecreasePowerMaxHeight = 0;
	lastTimeDecreasePowerIdle = 0;

	isAttacking = false;

	tail = new Tail();
	LPSCENE scene = Game::GetInstance()->GetCurrentScene();
	((ScenePlayer*)scene)->AddObject(tail);
}

void Mario::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	// Calculate dx, dy 
	GameObject::Update(dt);

	Movement();
	Action();
	ManagePowerDuration();

	std::vector<LPCOLLISIONEVENT> coEvents;
	std::vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != MARIO_DEAD)
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

		FilterCollision(coEvents, coEventsResult, min_tx, min_ty, nx, ny, rdx, rdy);

		// how to push back Mario if collides with a moving objects, what if Mario is pushed this way into another object?
		if (rdx != 0 && rdx != dx)
			x += nx * abs(rdx);

		// block 
		x += min_tx * dx + nx * PUSH_BACK;		// nx*0.4f : need to push out a bit to avoid overlapping next frame
		y += min_ty * dy + ny * PUSH_BACK;

		float entry_vx = vx;
		float entry_vy = vy;
		if (nx != 0) vx = 0;
		if (ny != 0) vy = 0;

		// Reset when touching the ground
		if (ny == -1) {
			UnsetAction(JUMPING);
			UnsetAction(DONE_JUMPING);
			UnsetAction(PEAKING);
			UnsetAction(DESCENDING);
		}

		//DebugOut(L"min_tx %f min_ty %f nx %f ny %f rdx %f rdy %f\n", min_tx, min_ty, nx, ny, rdx, rdy);

	   //
	   // Collision logic with other objects
	   //
		for (size_t i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<Goomba*>(e->obj)) // if e->obj is Goomba 
			{
				vx = entry_vx;
				x -= min_tx * dx + nx * PUSH_BACK;
				x += dx;

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
							if (state == MARIO_SMALL)
							{
								state = MARIO_SMALL;
								StartUntouchable();
							}
							else
							{
								if (state == MARIO_BIG) SetState(MARIO_BIG_TO_SMALL);
								else SetState(MARIO_DEAD);
							}
						}
					}
				}
			}

			else if (dynamic_cast<Platform*>(e->obj))
			{
				// Stop when horizontally hit a platform
				if (e->nx != 0 && min_ty == 1) ax = 0;
				if (e->ny != 0) vy = 0;
				// Stop jumping when hit an object above Mario
				if (e->ny == 1) SetAction(DONE_JUMPING);
			}

			else if (dynamic_cast<Brick*>(e->obj))
			{
				if (e->nx != 0 && min_ty == 1) ax = 0;

				Brick* b = dynamic_cast<Brick*>(e->obj);

				if (e->ny == 1) {
					SetAction(DONE_JUMPING);
					b->Hit();
				}
			}

			else if (dynamic_cast<Block*>(e->obj))
			{
				vx = entry_vx;
				x -= min_tx * dx + nx * PUSH_BACK;
				x += dx;
				if (e->ny == 1) {
					vy = entry_vy;
					y -= min_ty * dy + ny * PUSH_BACK;
					y += dy;
				}
			}

			else if (dynamic_cast<Mushroom*>(e->obj))
			{
				vx = entry_vx;
				x -= min_tx * dx + nx * PUSH_BACK;
				x += dx;

				if (e->ny == 1)
				{
					vy = entry_vy;
					y -= min_ty * dy + ny * PUSH_BACK;
					y += dy;
				}

				Mushroom* m = dynamic_cast<Mushroom*>(e->obj);
				m->AddPoint();
				m->Disable();
				if (state == MARIO_SMALL) SetState(MARIO_SMALL_TO_BIG);
			}

			else if (dynamic_cast<Portal*>(e->obj))
			{
				Portal* p = dynamic_cast<Portal*>(e->obj);
				Game::GetInstance()->SwitchScene(p->GetSceneId());
			}

			else
			{
				DebugOut(L"[INFO] Collided with an unspecified object!\n");
			}
		}
	}
	// clean up collision events
	for (size_t i = 0; i < coEvents.size(); i++) delete coEvents[i];
	//DebugOut(L"x %f nx %d ax %f vx %f \n", x, nx, ax, vx);
	//DebugOut(L"y %f ny %d ay %f vy %f \n", y, nx, ay, vy);
}

void Mario::Render()
{
	int ani = NULL;
	D3DXVECTOR2 translation = D3DXVECTOR2(0, 0);

	if (state == MARIO_DEAD)
		ani = ANI_DEAD;
	else
		switch (state) {

		case MARIO_SMALL:
			translation.x = -MARIO_SMALL_TRANSLATE_X;
			if (ax != 0 || AS_SHORT(movement))
			{
				ani = ANI_SMALL_WALKING;
				if (powerMeter > 2) ani = ANI_SMALL_RUNNING;
				if (powerMeter == MAX_POWER_METER) ani = ANI_SMALL_RUNNING_MAX;

				if (ax * nx < 0 && AS_SHORT(movement)) ani = ANI_SMALL_BRAKING;
			}
			else ani = ANI_SMALL_IDLE;
			if (GetAction(JUMPING))
			{
				ani = ANI_SMALL_JUMPING;
				if (powerMeter == MAX_POWER_METER) ani = ANI_SMALL_JUMPING_MAX;
			}
			break;

		case MARIO_BIG:
			translation.x = nx < 0 ? 0 : -MARIO_BIG_TRANSLATE_X;
			if (ax != 0 || AS_SHORT(movement))
			{
				ani = ANI_BIG_WALKING;
				if (powerMeter > 2) ani = ANI_BIG_RUNNING;
				if (powerMeter == MAX_POWER_METER) ani = ANI_BIG_RUNNING_MAX;

				if (ax * nx < 0 && AS_SHORT(movement)) ani = ANI_BIG_BRAKING;
			}
			else ani = ANI_BIG_IDLE;

			if (GetAction(JUMPING)) {
				if (vy < 0) ani = ANI_BIG_JUMPING;
				else ani = ANI_BIG_JUMPED;
				if (powerMeter == MAX_POWER_METER) ani = ANI_BIG_JUMPING_MAX;
			}

			if (GetAction(DUCKING)) {
				ani = ANI_BIG_DUCKING;
				translation.x = nx < 0 ? 0 : -MARIO_BIG_TRANSLATE_X + 3;
			}
			break;

		case MARIO_RACOON:
			translation.x = nx < 0 ? 0 : -MARIO_RACOON_TRANSLATE_X;
			if (ax != 0 || AS_SHORT(movement))
			{
				ani = ANI_RACOON_WALKING;
				if (powerMeter > 2) ani = ANI_RACOON_RUNNING;
				if (powerMeter == MAX_POWER_METER) ani = ANI_RACOON_RUNNING_MAX;
				translation.x = nx < 0 ? -1 : -MARIO_RACOON_TRANSLATE_X + 1;

				if (ax * nx < 0 && AS_SHORT(movement)) {
					ani = ANI_RACOON_BRAKING;
					translation.x = -1;
				}
			}
			else ani = ANI_RACOON_IDLE;

			if (GetAction(JUMPING)) {
				if (vy < 0) ani = ANI_RACOON_JUMPING;
				// else ani = ANI_RACOON_JUMPED;
				if (powerMeter == MAX_POWER_METER) ani = ANI_RACOON_JUMPING_MAX;
			}
			if (GetAction(FLYING))
			{
				ani = ANI_RACOON_FLYING;
				translation.x = nx < 0 ? 0 : -MARIO_RACOON_TRANSLATE_X;
			}
			if (GetAction(DESCENDING) && vy > 0)
			{
				ani = ANI_RACOON_DESCENDING;
				translation.x = nx < 0 ? -1 : -MARIO_RACOON_TRANSLATE_X + 1;
			}
			if (GetAction(DUCKING))
			{
				ani = ANI_RACOON_DUCKING;
				translation.x = nx < 0 ? 0 : -MARIO_RACOON_TRANSLATE_X + 3;
			}
			if (GetAction(SPINNING) && !GetAction(DUCKING)) {
				ani = ANI_RACOON_SPINNING;
				translation.x = nx < 0 ? 0 : -MARIO_RACOON_TRANSLATE_X;
			}
			break;

		case MARIO_SMALL_TO_BIG:
			ani = ANI_SMALL_TO_BIG;
			break;

		case MARIO_BIG_TO_SMALL:
			ani = ANI_BIG_TO_SMALL;
			break;
		}

	int alpha = 255;
	if (untouchable) alpha = 128;

	animation_set->at(ani)->
		Render(nx, (x), ceil(y), alpha, translation);

	RenderBoundingBox();
}

void Mario::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case MARIO_DEAD:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	case MARIO_SMALL_TO_BIG:
		// Subtract 0.1f to make sure Mario doesn't overlap with platform
		y -= MARIO_BIG_HEIGHT - MARIO_SMALL_HEIGHT - 0.1f;
		animationTimer = GetTickCount64();
		Game::GetInstance()->Pause();
		break;
	case MARIO_BIG_TO_SMALL:
		animationTimer = GetTickCount64();
		Game::GetInstance()->Pause();
		break;
	}
}

void Mario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	// - nx : allow Mario to get really close to objects
	left = x;
	top = y;

	switch (state)
	{
	case MARIO_SMALL:
		right = x + MARIO_SMALL_WIDTH;
		bottom = y + MARIO_SMALL_HEIGHT;
		break;
	case MARIO_BIG:
		right = x + MARIO_BIG_WIDTH;
		if (GetAction(DUCKING)) bottom = y + MARIO_DUCKING_HEIGHT;
		else bottom = y + MARIO_BIG_HEIGHT;
		break;
	case MARIO_RACOON:
		right = x + MARIO_BIG_WIDTH;
		if (GetAction(DUCKING)) bottom = y + MARIO_DUCKING_HEIGHT;
		else bottom = y + MARIO_RACOON_HEIGHT;
		break;
	default:
		right = x;
		bottom = y;
		break;
	}
}

/*
	Reset Mario status to the beginning state of a scene
*/
void Mario::Reset()
{
	SetState(MARIO_BIG);
	SetPosition(start_x, start_y);
	SetSpeed(0, 0);
}

void Mario::Movement()
{
	// Velocity with friction + power meter
	vx = state == MARIO_SMALL ?
		(MARIO_WALKING_SPEED_SMALL + powerMeter * MARIO_POWER_ACCELERATION) * ax :
		(MARIO_WALKING_SPEED + powerMeter * MARIO_POWER_ACCELERATION) * ax;
	// Simple fall down
	vy += GLOBAL_GRAVITY * dt;
	// Max falling speed
	if (vy > GLOBAL_TERMINAL_VELOCITY) vy = GLOBAL_TERMINAL_VELOCITY;


	if (GetMovement(LEFT))
	{
		nx = -1;
		ax -= MARIO_ACCELERATION_X;
	}
	else if (GetMovement(RIGHT))
	{
		nx = 1;
		ax += MARIO_ACCELERATION_X;
	}

	if (GetMovement(UP))
	{
		if (GetAction(SUPER_JUMPING)) {
			if (!GetAction(JUMPING))
			{
				last_y = y;
				vy = -MARIO_JUMP_SPEED_HIGH;
			}
			float maxHeight = level == MARIO_SMALL ? 30.0f : MARIO_JUMP_HEIGHT_MAX;
			if (!GetAction(DONE_JUMPING) &&
				last_y - y < maxHeight + powerMeter * MARIO_JUMP_HEIGHT_POWER) {
				vy -= MARIO_ACCELERATION_Y;
				// Gravity compensation
				vy -= MARIO_GRAVITY * dt;
			}
			else
			{
				SetAction(DONE_JUMPING);
				// Prevent super jumping repeatedly, 
				// just like the original game
				UnsetMovement(UP);
			}
		}
		else
		{
			if (!GetAction(JUMPING)) {
				vy = -MARIO_JUMP_SPEED_LOW;
				if (!GetAction(FLYING)) vy -= powerMeter == MAX_POWER_METER ? MARIO_POWER_JUMP : 0;

				// Set flyTimer when start jumping
				flyTimer = 0;
			}

			if (state == MARIO_RACOON)
			{
				if (GetAction(FLYING) && powerMeter == MAX_POWER_METER)
				{
					// Move left/right slower 
					ax *= MARIO_FLY_ACCELERATION_X;

					flyTimer += dt;
					if (flyTimer < POWER_MAX_FLY_TIME)
					{
						// Jump before taking off 
						// with a little jump speed easing
						if (vy < MARIO_VELOCITY_Y_THRESHOLD) vy += MARIO_FLYING_GRAVITY;
						vy -= MARIO_FLY_SPEED;
						vy -= MARIO_GRAVITY * dt;
					}
					else {
						UnsetAction(FLYING);
						SetAction(PEAKING);
					};
				}
			}
		}

		SetAction(JUMPING);
	}

	if (GetMovement(DOWN))
	{
		// Slow down when ducking
		// Check if both movement[LEFT] and movement[LEFT] equal 0
		if (state != MARIO_SMALL && !AS_SHORT(movement)) {
			if (ax < 0) {
				ax += MARIO_ACCELERATION_X;
				ax -= powerMeter * MARIO_POWER_INERTIA;
			}
			else if (ax > 0) {
				ax -= MARIO_ACCELERATION_X;
				ax += powerMeter * MARIO_POWER_INERTIA;
			}
			// Teleport to ducking position for accurate hitboxes
			// Prevent ducking mid-air
			if (!GetAction(DUCKING) && !GetAction(JUMPING) && vy <= MARIO_DEFAULT_VELOCITY_THRESHOLD)
			{
				ShiftPosition(DUCKING, SHIFT);
				SetAction(DUCKING);
			}
		}
		// Disable ducking when moving
		if (AS_SHORT(movement))
		{
			if (GetAction(DUCKING)) ShiftPosition(DUCKING, UNSHIFT);
			UnsetAction(DUCKING);
		}
	}
	else if (GetAction(DUCKING)) {
		// Teleport back to normal position
		ShiftPosition(DUCKING, UNSHIFT);
		UnsetAction(DUCKING);
	}

	

	// Mario Inertia
	// Disabled when moving left to prevent it slow down left movement
	if (!GetAction(FLYING) && ax < 0 && !GetMovement(LEFT))
	{
		// Let Mario keep his horizontal momentum when airborne
		//if (vy <= MARIO_AIRBORNE_MINIMUM_THRESHOLD)
		ax += (state == MARIO_SMALL ? MARIO_INERTIA_SMALL : MARIO_INERTIA);
		ax -= powerMeter * MARIO_POWER_INERTIA;
	}
	if (!GetAction(FLYING) && ax > 0 && !GetMovement(RIGHT))
	{
		//if (vy <= MARIO_AIRBORNE_MINIMUM_THRESHOLD)
		ax -= (state == MARIO_SMALL ? MARIO_INERTIA_SMALL : MARIO_INERTIA);
		ax += powerMeter * MARIO_POWER_INERTIA;
	}

	// Equalize acceleration to 0 if there's little acceleration
	if (abs(ax) <= MARIO_INERTIA) ax = 0;
	// ax = 1 or ax = -1 are max acceleration allowed
	else if (ax > 1) ax = 1;
	else if (ax < -1) ax = -1;
}

void Mario::Action()
{
	switch (state)
	{
	case MARIO_SMALL_TO_BIG:
		if (GetTickCount64() - animationTimer >= SMALL_TO_BIG_DURATION) {
			Game::GetInstance()->Unpause();
			SetState(MARIO_BIG);
		}
		break;

	case MARIO_BIG_TO_SMALL:
		if (GetTickCount64() - animationTimer >= BIG_TO_SMALL_DURATION) {
			Game::GetInstance()->Unpause();
			SetState(MARIO_SMALL);
			y += MARIO_BIG_HEIGHT - MARIO_SMALL_HEIGHT + 0.1f;
		}
		break;
	case MARIO_RACOON:
		if (GetAction(DESCENDING))
		{
			// Set vy to a constant when falling down
			if (vy > 0) vy = MARIO_DESCENDING_SPEED;
		};

		if (GetAction(SPINNING) && !GetAction(DUCKING)) {
			tail->Enable();
			tail->SetDirection(nx);
			tail->SetPosition(x, y);
		}
		else tail->Disable();
		break;
	}
}

void Mario::ManagePowerDuration()
{
	powerTimer += dt;

	if (!GetAction(FLYING))
	{
		// Power decrease slower
		if (!AS_SHORT(movement) || (nx * ax < 1)) {
			if (powerTimer - lastTimeDecreasePowerIdle >= POWER_DOWN_DURATION_STEP)
			{
				if (powerMeter > 0) {
					powerMeter--;
					lastTimeDecreasePowerIdle = powerTimer;
				}
			}
		}

		int duration;
		// If Mario is moving in an unchaged direction
		if (nx * ax > 0) duration = POWER_DIRECTION_UNCHANGED_STEP;
		else duration = POWER_DIRECTION_CHANGED_STEP;
		if (powerTimer - lastTimeGainPower >= duration)
		{
			// Only increase power when moving left or right
			if (abs(ax) >= POWER_ALLOW_GAINING_THRESHOLD && powerMeter >= 0)
			{
				if (GetAction(GAINING_POWER) && AS_SHORT(movement) &&
					nx * ax > 0 && powerMeter < MAX_POWER_METER) powerMeter++;

				else if (powerMeter > 0 && nx * ax < POWER_ALLOW_GAINING_THRESHOLD) powerMeter--;
				lastTimeGainPower = powerTimer;
			}
		}
	}

	// Decrease power quickly when maximum flying height reached
	if (GetAction(PEAKING) && powerMeter > 0)
	{
		if (powerTimer - lastTimeDecreasePowerMaxHeight >= POWER_PEAKED_DECREASE_TIME)
		{
			powerMeter--;
			lastTimeDecreasePowerMaxHeight = powerTimer;
		}
	}
}

void Mario::ShiftPosition(int action, int sign)
{
	if (action == DUCKING)
	{
		if (state == MARIO_BIG) y += BIG_BBOX_DUCKING_DIFF * sign;
		else if (state == MARIO_RACOON) y += RACOON_BBOX_DUCKING_DIFF * sign;
	}
}