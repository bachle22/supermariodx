#include "Platform.h"

Platform::Platform()
{
	isPressed = false;
	timer = 0;
}

void Platform::Render()
{
	RenderBoundingBox();
	if (isPressed) RenderBoundingBox();
}

void Platform::Update(ULONGLONG dt, std::vector<LPGAMEOBJECT>* coObjects)
{
	if (isPressed) timer += dt;
	if (timer > PLATFORM_UNPRESS_DELAY) Unpress();
}

void Platform::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + TILE_WIDTH;
	bottom = y + TILE_WIDTH;
}

void Platform::Press()
{
	timer = 0;
	if (!isPressed) {
		y += 0.01f;
		isPressed = true;
	}
}

void Platform::Unpress()
{
	if (isPressed) {
		y -= 0.01f;
		isPressed = false;
		timer = 0;
	}
}