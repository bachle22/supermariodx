#pragma once

#include <algorithm>
#include <vector>

#include "Game.h"
#include "GameObject.h"
#include "Sprites.h"
#include "Textures.h"
#include "Collision.h"
#include "Definition.h"
#include "Debug.h"


GameObject::GameObject()
{
	x = y = 0;
	vx = vy = 0;
	nx = 1;
	isEnabled = true;
	priority = PRIORITY_LOW;
}


GameObject::~GameObject()
{
}

void GameObject::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	this->dt = dt;
	dx = vx * dt;
	dy = vy * dt;
}

void GameObject::RenderBoundingBox()
{
	if (!Game::GetInstance()->IsKeyDown(DIK_B)) return;
	D3DXVECTOR3 p(x, y, 0);
	RECT rect;

	LPDIRECT3DTEXTURE9 bbox = Textures::GetInstance()->Get(ID_TEX_BBOX);

	float l, t, r, b;

	GetBoundingBox(l, t, r, b);
	rect.left = 0;
	rect.top = 0;
	rect.right = (int)r - (int)l;
	rect.bottom = (int)b - (int)t;

	Game::GetInstance()->Draw(nx, ceil(x), ceil(y), bbox, rect.left, rect.top, rect.right, rect.bottom, 64);
}

/*
	Calculate potential collisions with the list of colliable objects

	coObjects: the list of colliable objects
	coEvents: list of potential collisions
*/
void GameObject::CalculatePotentialCollisions(
	std::vector<LPGAMEOBJECT>* coObjects,
	std::vector<LPCOLLISIONEVENT>& coEvents)
{
	for (size_t i = 0; i < coObjects->size(); i++)
	{
		LPCOLLISIONEVENT e = SweptAABBEx(coObjects->at(i));
		if (e->t > 0 && e->t <= 1.0f)
			coEvents.push_back(e);
		else
			delete e;
	}

	std::sort(coEvents.begin(), coEvents.end(), CollisionEvent::compare);
}

void GameObject::FilterCollision(
	std::vector<LPCOLLISIONEVENT>& coEvents,
	std::vector<LPCOLLISIONEVENT>& coEventsResult,
	float& min_tx, float& min_ty,
	float& nx, float& ny, float& rdx, float& rdy)
{
	min_tx = 1.0f;
	min_ty = 1.0f;
	int min_ix = -1;
	int min_iy = -1;

	nx = 0.0f;
	ny = 0.0f;

	coEventsResult.clear();

	for (size_t i = 0; i < coEvents.size(); i++)
	{
		LPCOLLISIONEVENT c = coEvents[i];

		if (c->t < min_tx && c->nx != 0) {
			min_tx = c->t; nx = c->nx; min_ix = i; rdx = c->dx;
		}

		if (c->t < min_ty && c->ny != 0) {
			min_ty = c->t; ny = c->ny; min_iy = i; rdy = c->dy;
		}
	}

	if (min_ix >= 0) coEventsResult.push_back(coEvents[min_ix]);
	if (min_iy >= 0) coEventsResult.push_back(coEvents[min_iy]);
}


/*
	Extension of original SweptAABB to deal with two moving objects
*/
LPCOLLISIONEVENT GameObject::SweptAABBEx(LPGAMEOBJECT coO)
{
	float sl, st, sr, sb;		// static object bbox
	float ml, mt, mr, mb;		// moving object bbox
	float t, nx, ny;

	coO->GetBoundingBox(sl, st, sr, sb);

	// deal with moving object: m speed = original m speed - collide object speed
	float svx, svy;
	coO->GetSpeed(svx, svy);

	float sdx = svx * dt;
	float sdy = svy * dt;

	// (rdx, rdy) is RELATIVE movement distance/velocity 
	float rdx = this->dx - sdx;
	float rdy = this->dy - sdy;

	GetBoundingBox(ml, mt, mr, mb);

	Collision::SweptAABB(
		ml, mt, mr, mb,
		rdx, rdy,
		sl, st, sr, sb,
		t, nx, ny
	);

	CollisionEvent* e = new CollisionEvent(t, nx, ny, rdx, rdy, coO);
	return e;
}

LPGAMEOBJECT GameObject::CheckOverlap(LPGAMEOBJECT coO)
{
	float l1, t1, r1, b1;		// static object bbox
	float l2, t2, r2, b2;		// moving object bbox

	coO->GetBoundingBox(l1, t1, r1, b1);
	GetBoundingBox(l2, t2, r2, b2);

	float left = l2 - r1;
	float top = b2 - t1;
	float right = r2 - l1;
	float bottom = t2 - b1;

	if (!(left > 0 || right < 0 || top < 0 || bottom > 0)) return coO;
	else return NULL;
}