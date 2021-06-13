#include "Game.h"
#include "Mario.h"
#include "Sprites.h"
#include "Animations.h"

Mario::Mario(float x, float y, float vx): GameObject(x, y)
{
	this->vx = vx;
};

void Mario::Update(DWORD dt)
{
	x += vx * dt;

	int BackBufferWidth = Game::GetInstance()->GetBackBufferWidth();
	if (x <= 0 || x >= BackBufferWidth - MARIO_WIDTH) {

		vx = -vx;

		if (x <= 0)
		{
			x = 0;
		}
		else if (x >= BackBufferWidth - MARIO_WIDTH)
		{
			x = (float)(BackBufferWidth - MARIO_WIDTH);
		}
	}
}

void Mario::Render()
{
	LPANIMATION ani;

	//[RED FLAG][TODO]: Associate this animation/asset to Mario!!
	if (vx > 0) ani = Animations::GetInstance()->Get(500);
	else ani = Animations::GetInstance()->Get(501);

	ani->Render(x, y);
}