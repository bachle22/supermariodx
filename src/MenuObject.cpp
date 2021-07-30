#include "MenuObject.h"
#include "SceneMenu.h"

MenuObject::MenuObject(int type)
{
	this->type = type;
	nx = -1;
}

void MenuObject::Render()
{
	animation_set->at(0)->Render(nx, x, y);
	RenderBoundingBox();
}

void MenuObject::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	GameObject::Update(dt, coObjects);

	x += dx;
	y += dy;

	if (type == OBJECT_TYPE_HAMMERBRO)
	{
		vx = nx * HAMMERBRO_SPEED;

		if (entryX - x >= 0) nx = 1;
		else if (x - entryX > HAMMERBRO_RANGE) nx = -1;
	}

}

void MenuObject::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{

}

void MenuObject::SetPosition(float x, float y)
{
	GameObject::SetPosition(x, y);
	entryX = x;
	entryY = y;
}

