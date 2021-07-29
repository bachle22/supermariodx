#include <vector>

#include "Mario.h"
#include "ScenePlayer.h"
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
#include "Projectile.h"
#include "Plant.h"
#include "Koopa.h"
#include "Warp.h"
#include "PSwitch.h"
#include "Roulette.h"
#include "Leaf.h"
#include "Transition.h"
#include "FloatingWood.h"

Mario::Mario(float x, float y) : GameObject()
{
	state = MARIO_SMALL;
	ax = 0; ay = 0;
	this->x = x;
	this->y = y;
	entryX = x;
	entryY = y;
	tempX = 0; tempY = 0;
	width = 0; height = 0;
	clipY = 0;

	powerMeter = 0;
	destSceneID = NULL;

	isUntouchable = false;
	untouchableTimer = 0;
	powerTimer = 0;
	flyTimer = 0;
	flashTimer = 0;

	lastTimeGainPower = 0;
	lastTimeDecreasePowerMaxHeight = 0;
	lastTimeDecreasePowerIdle = 0;

	tail = new Tail();
	LPSCENE scene = Game::GetInstance()->GetCurrentScene();
	((ScenePlayer*)scene)->AddObject(tail);
}

void Mario::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	// Calculate dx, dy 
	GameObject::Update(dt);
	if (tail->IsEnabled()) tail->Update(dt, coObjects);

	Movement();
	UpdateState();
	ManagePowerDuration();


	// reset untouchable timer if untouchable time has passed
	if (isUntouchable)
	{
		untouchableTimer += dt;
		if (untouchableTimer > MARIO_UNTOUCHABLE_TIME)
		{
			untouchableTimer = 0;
			isUntouchable = 0;
		}
	}

	std::vector<LPCOLLISIONEVENT> coEvents;
	std::vector<LPCOLLISIONEVENT> coEventsResult;

	coEvents.clear();

	// turn off collision when die 
	if (state != MARIO_DEAD) CalculatePotentialCollisions(coObjects, coEvents);

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
		/*if (rdx != 0 && rdx != dx)
			x += nx * abs(rdx);*/

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
			UnsetAction(ON_PORTAL);
		}

		//DebugOut(L"min_tx %f min_ty %f nx %f ny %f rdx %f rdy %f\n", min_tx, min_ty, nx, ny, rdx, rdy);

	   //
	   // Collision logic with other objects
	   //
		for (size_t i = 0; i < coEventsResult.size(); i++)
		{
			LPCOLLISIONEVENT e = coEventsResult[i];

			if (dynamic_cast<Platform*>(e->obj))
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
					y -= min_ty * dy + ny * PUSH_BACK - dy;
				}
			}

			else if (dynamic_cast<FloatingWood*>(e->obj))
			{
				if (e->ny == -1) {
					SetAction(TOUCHING_WOOD);
					FloatingWood* w = dynamic_cast<FloatingWood*>(e->obj);
					w->SetState(FLOATING_WOOD_FALLING);
				}
				if (e->ny == 1) SetAction(DONE_JUMPING);
			}

			else if (dynamic_cast<Portal*>(e->obj))
			{
				SetAction(ON_PORTAL);
				Portal* p = dynamic_cast<Portal*>(e->obj);
				if (e->ny != 0)
				{

					if (GetAction(ACTIVATING_PORTAL))
					{
						SetAction(ENTERING_PORTAL);
						UnsetAction(EXITED_PORTAL);
						p->GetDestination(tempX, tempY);
						if (e->ny == -1)
						{
							SetAction(MOVING_DOWN);
							UnsetAction(MOVING_UP);
						}
						else
						{
							SetAction(MOVING_UP);
							UnsetAction(MOVING_DOWN);
						}
						destSceneID = p->GetSceneID();
					}

					if (e->ny == 1)
					{
						SetAction(DONE_JUMPING);
					}
				}
			}

			else if (dynamic_cast<Mushroom*>(e->obj))
			{
				vx = entry_vx;
				x -= min_tx * dx + nx * PUSH_BACK - dx;

				if (ny != -1)
				{
					vy = entry_vy;
					y -= min_ty * dy + ny * PUSH_BACK - dy;
				}

				Mushroom* m = dynamic_cast<Mushroom*>(e->obj);
				m->AddPoint();
				m->Disable();
				if (state == MARIO_SMALL) SetState(MARIO_SMALL_TO_BIG);
			}


			else if (dynamic_cast<Coin*>(e->obj))
			{
				vx = entry_vx;
				x -= min_tx * dx + nx * PUSH_BACK - dx;
				if (e->nx == 0)
				{
					vy = entry_vy;
					y -= min_ty * dy + ny * PUSH_BACK - dy;
				}
				dynamic_cast<Coin*>(e->obj)->Earn();
			}

			else if (dynamic_cast<Leaf*>(e->obj))
			{
				vx = entry_vx;
				x -= min_tx * dx + nx * PUSH_BACK - dx;
				if (e->nx == 0 && e->ny == 1)
				{
					vy = entry_vy;
					y -= min_ty * dy + ny * PUSH_BACK - dy;
				}
				SetState(MARIO_BIG_TO_RACOON);

				dynamic_cast<Leaf*>(e->obj)->Consume();
			}

			else if (dynamic_cast<Goomba*>(e->obj))
			{
				vx = entry_vx;
				x -= min_tx * dx + nx * PUSH_BACK - dx;

				Goomba* goomba = dynamic_cast<Goomba*>(e->obj);

				// jump on top >> kill Goomba and deflect a bit 
				if (e->ny < 0)
				{
					if (goomba->GetState() != GOOMBA_STATE_DIE)
					{
						goomba->Downgrade();
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
				}
				else if (goomba->GetState() != GOOMBA_STATE_DIE)
				{
					this->Downgrade();
				}

			}

			else if (dynamic_cast<PSwitch*>(e->obj))
			{
				PSwitch* p = dynamic_cast<PSwitch*>(e->obj);
				if (e->ny < 0) p->Switch();
				vy = -MARIO_JUMP_DEFLECT_SPEED;
			}

			else if (dynamic_cast<Roulette*>(e->obj))
			{
				vx = entry_vx;
				x -= min_tx * dx + nx * PUSH_BACK - dx;
				vy = entry_vy;
				y -= min_ty * dy + ny * PUSH_BACK - dy;

				dynamic_cast<Roulette*>(e->obj)->Withdraw();
			}

			else if (dynamic_cast<Koopa*>(e->obj))
			{
				vx = entry_vx;
				x -= min_tx * dx + nx * PUSH_BACK - dx;

				Koopa* k = dynamic_cast<Koopa*>(e->obj);

				if (e->ny < 0)
				{
					if (k->GetState() != KOOPA_STATE_HIDING)
					{
						k->Downgrade();
						vy = -MARIO_JUMP_DEFLECT_SPEED;
					}
					else
					{
						vy = entry_vy;
						y -= min_ty * dy + ny * PUSH_BACK - dy;

						SetAction(KICKING);
						k->SetDirection(this->nx);
						k->Downgrade();
					}
				}
				else if (e->nx != 0)
				{
					if (k->GetState() != KOOPA_STATE_HIDING &&
						k->GetState() != KOOPA_STATE_REVIVING)
						Downgrade();
					else {
						SetAction(KICKING);
						k->SetDirection(this->nx);
						k->Downgrade();
					}
				}
			}

			else if (dynamic_cast<Projectile*>(e->obj) || dynamic_cast<Plant*>(e->obj))
			{
				vx = entry_vx;
				x -= min_tx * dx + nx * PUSH_BACK - dx;
				vy = entry_vy;
				y -= min_ty * dy + ny * PUSH_BACK - dy;
				Downgrade();

			}

			else
			{
				DebugOut(L"[INFO] Collided with an unspecified object!\n");
			}
		}
	}
	// clean up collision events
	for (size_t i = 0; i < coEvents.size(); i++) delete coEvents[i];
}

void Mario::Render()
{
	int ani = NULL;
	D3DXVECTOR2 translation = D3DXVECTOR2(0, 0);

	if (state == MARIO_DEAD) ani = ANI_DEAD;
	else switch (state) {
	case MARIO_SMALL:
		translation.x = -MARIO_SMALL_TRANSLATE_X;
		translation.y = -1;
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
		if (GetAction(ENTERING_PORTAL) || GetAction(LEAVING_PORTAL))
		{
			ani = ANI_SMALL_PIPE;
			translation.y = 0;
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
			translation.x = nx < 0 ? 0 : -MARIO_BIG_TRANSLATE_X + 2;
		}

		if (GetAction(KICKING)) {
			ani = ANI_BIG_KICKING;
			translation.x = nx < 0 ? 0 : -MARIO_BIG_TRANSLATE_X + 2;
		}

		if (GetAction(ENTERING_PORTAL) || GetAction(LEAVING_PORTAL))
		{
			ani = ANI_BIG_PIPE;
			translation.x = -1;
		}

		break;

	case MARIO_RACOON:
		translation.x = nx < 0 ? 0 : -MARIO_RACOON_TRANSLATE_X;
		if ((ax != 0 || AS_SHORT(movement)))
		{
			ani = ANI_RACOON_WALKING;
			if (powerMeter > 2) ani = ANI_RACOON_RUNNING;
			if (powerMeter == MAX_POWER_METER) ani = ANI_RACOON_RUNNING_MAX;
			translation.x = nx < 0 ? -1 : -MARIO_RACOON_TRANSLATE_X + 1;

			if (ax * nx < 0 && AS_SHORT(movement) && !GetAction(JUMPING)) {
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

		if (GetAction(ENTERING_PORTAL) || GetAction(LEAVING_PORTAL))
		{
			ani = ANI_BIG_PIPE;
			translation.x = -1;
		}

		break;

	case MARIO_SMALL_TO_BIG:
		ani = ANI_SMALL_TO_BIG;
		break;

	case MARIO_BIG_TO_SMALL:
		ani = ANI_BIG_TO_SMALL;
		break;

	case MARIO_BIG_TO_RACOON:
		ani = ANI_BIG_IDLE;
		break;
	case MARIO_RACOON_TO_BIG:
		ani = ANI_BIG_IDLE;
		break;
	}

	int alpha = VISIBLE;
	if (isUntouchable)
	{
		flashTimer += dt;
		if (flashTimer < MARIO_FLASH_INTERVEL) alpha = INVISIBLE;
		else if (flashTimer < MARIO_FLASH_INTERVEL * 2) alpha = VISIBLE;
		else flashTimer = 0;
	}

	if (GetAction(ENTERING_PORTAL))
	{
		if (GetAction(MOVING_DOWN))
		{
			animation_set->at(ani)->Render(
				nx, x, ceil(y), VISIBLE, translation,
				0, y - clipY,
				TILE_WIDTH, height + y - clipY);
		}
		else if (GetAction(MOVING_UP))
		{
			animation_set->at(ani)->Render(
				nx, x, ceil(y), VISIBLE, translation,
				0, y - clipY,
				TILE_WIDTH, height + 1);
		}
	}
	else if (GetAction(LEAVING_PORTAL))
	{
		if (GetAction(MOVING_DOWN))
		{
			animation_set->at(ani)->Render(
				nx, x, ceil(y), VISIBLE, translation,
				0, clipY,
				TILE_WIDTH, height);
		}
		else if (GetAction(MOVING_UP))
		{
			animation_set->at(ani)->Render(
				nx, x, ceil(y), VISIBLE, translation,
				0, -clipY,
				TILE_WIDTH, height - clipY);
		}
	}
	else if (Game::GetInstance()->IsPaused() && state <= MARIO_RACOON)
	{
		animation_set->at(ani)->RenderFirstFrame(nx, (x), ceil(y), translation);
	}
	else
	{
		animation_set->at(ani)->Render(nx, (x), ceil(y), alpha, translation);
		/*animation_set->at(ani)->Render(
			nx, x, ceil(y), VISIBLE, translation,
			0, Game::GetInstance()->DEBUG_X,
			TILE_WIDTH, height + Game::GetInstance()->DEBUG_Y);*/
	}

	RenderBoundingBox();
	tail->Render();
}


void Mario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
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
		if (GetAction(DUCKING) && !GetAction(ON_PORTAL)) bottom = y + MARIO_DUCKING_HEIGHT;
		else bottom = y + MARIO_BIG_HEIGHT;
		break;
	case MARIO_RACOON:
		right = x + MARIO_BIG_WIDTH;
		if (GetAction(DUCKING) && !GetAction(ON_PORTAL)) bottom = y + MARIO_DUCKING_HEIGHT;
		else bottom = y + MARIO_RACOON_HEIGHT;
		break;
	default:
		right = x;
		bottom = y;
		break;
	}

	width = right - left;
	height = bottom - top;

}

/*
	Reset Mario status to the beginning state of a scene
*/
void Mario::Reset()
{
	SetState(MARIO_SMALL);
	SetPosition(entryX, entryY);
	SetSpeed(0, 0);
}

void Mario::Movement()
{
	if (GetAction(ENTERING_PORTAL)) EnterPortal();
	else if (GetAction(LEAVING_PORTAL)) LeavePortal();

	if (Game::GetInstance()->IsPaused()) return;

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
		UnsetAction(TOUCHING_WOOD);

		if (GetAction(SUPER_JUMPING)) {
			if (!GetAction(JUMPING))
			{
				beforeJumpingY = y;
				vy = -MARIO_JUMP_SPEED_HIGH;
			}
			float maxHeight = level == MARIO_SMALL ? 30.0f : MARIO_JUMP_HEIGHT_MAX;
			if (!GetAction(DONE_JUMPING) &&
				beforeJumpingY - y < maxHeight + powerMeter * MARIO_JUMP_HEIGHT_POWER) {
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
			if (!GetAction(DUCKING) && !GetAction(JUMPING) && !GetAction(ON_PORTAL)
				&& vy <= MARIO_DEFAULT_VELOCITY_THRESHOLD)
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


void Mario::SetState(int state)
{
	GameObject::SetState(state);
	switch (state)
	{
	case MARIO_DEAD:
		vy = -MARIO_DIE_DEFLECT_SPEED;
		break;
	case MARIO_SMALL_TO_BIG:
		// Making sure Mario doesn't overlap with platform
		y -= MARIO_BIG_HEIGHT - MARIO_SMALL_HEIGHT + 0.5f;
		StartAnimationTimer();
		Game::GetInstance()->Pause();
		break;
	case MARIO_BIG_TO_SMALL:
		StartAnimationTimer();
		Game::GetInstance()->Pause();
		break;
	case MARIO_BIG_TO_RACOON:
	{
		StartAnimationTimer();
		Warp* warp = new Warp(x, y + MARIO_WARP_EFFECT_Y);
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->AddObject(warp);
		Game::GetInstance()->Pause();
		break;
	}
	case MARIO_RACOON_TO_BIG:
	{
		tail->Disable();
		StartAnimationTimer();
		Warp* warp = new Warp(x, y + MARIO_WARP_EFFECT_Y);
		LPSCENE scene = Game::GetInstance()->GetCurrentScene();
		((ScenePlayer*)scene)->AddObject(warp);
		Game::GetInstance()->Pause();
		break;
	}
	}
}

void Mario::UpdateState()
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
			y += MARIO_BIG_HEIGHT - MARIO_SMALL_HEIGHT - 1.f;
			SetUntouchable();
		}
		break;
	case MARIO_BIG_TO_RACOON:
		if (GetTickCount64() - animationTimer >= BIG_TO_RACOON_DURATION) {
			Game::GetInstance()->Unpause();
			SetState(MARIO_RACOON);
			y -= 1.f;
			SetUntouchable();
		}
		break;
	case MARIO_RACOON_TO_BIG:
		if (GetTickCount64() - animationTimer >= RACOON_TO_BIG_DURATION) {
			Game::GetInstance()->Unpause();
			SetState(MARIO_BIG);
			SetUntouchable();
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

void Mario::Downgrade()
{
	if (isUntouchable) return;
	if (GetAction(DUCKING))
	{
		UnsetAction(DUCKING);
		ShiftPosition(DUCKING, UNSHIFT);
	}

	switch (state)
	{
	case MARIO_RACOON:
		SetState(MARIO_RACOON_TO_BIG);
		break;
	case MARIO_BIG:
		SetState(MARIO_BIG_TO_SMALL);
		break;
	case MARIO_SMALL:
		SetState(MARIO_DEAD);
		break;
	}
}

void Mario::EnterPortal()
{
	if (clipY == 0) clipY = y;

	if (abs(y - clipY) <= height)
	{
		if (GetAction(MOVING_DOWN)) clipY += MARIO_PIPE_SPEED;
		if (GetAction(MOVING_UP)) clipY -= MARIO_PIPE_SPEED;
		Game::GetInstance()->Pause();
	}
	else if (abs(y - clipY) > height && !GetAction(EXITED_PORTAL))
	{
		Transition::GetInstance()->FadeIn();
		SetAction(EXITED_PORTAL);
	}
	else if (Transition::GetInstance()->IsFinished())
	{
		UnsetAction(ENTERING_PORTAL);
		y = 0;
		clipY = height;
		if (destSceneID == NULL) return;
		if (GetAction(MOVING_DOWN))
			Game::GetInstance()->FastSwitchScene(destSceneID, tempX, tempY);
		else if (GetAction(MOVING_UP))
			Game::GetInstance()->FastSwitchScene(destSceneID, tempX, tempY - height);
	}

}

void Mario::LeavePortal()
{
	//if (clipY == 0) clipY = y;

	if (clipY > 0)
	{
		Game::GetInstance()->Pause();
		clipY -= MARIO_PIPE_SPEED;
		UnsetAction(EXITED_PORTAL);
	}
	else
	{
		UnsetAction(LEAVING_PORTAL);
		Game::GetInstance()->Unpause();
		clipY = 0;
	}
}