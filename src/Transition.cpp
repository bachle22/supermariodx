#include "Game.h"
#include "Transition.h"
#include "Definition.h"
#include "Camera.h"
#include "Debug.h"

Transition* Transition::__instance = NULL;

Transition::Transition()
{
	type = TRANSITION_UNDEFINED;
	x = 0;
	y = 0;
	isFinished = true;
	background = Textures::GetInstance()->Get(ID_TEX_BACKGROUND);
}

Transition* Transition::GetInstance()
{
	if (__instance == NULL) __instance = new Transition();
	return __instance;
}

void Transition::Render()
{	
	if (type == TRANSITION_UNDEFINED) return;

	//if (!isFinished) Game::GetInstance()->Pause();
	//else if (Game::GetInstance()->IsPaused()) Game::GetInstance()->Unpause();

	if (type == FADE_IN && opacity < 255) opacity += TRANSITION_STEP;
	else if (type == FADE_OUT && opacity > 0) opacity -= TRANSITION_STEP;
	else
	{
		isFinished = true;
		type = TRANSITION_UNDEFINED;
	}

	if (opacity < 0) opacity = 0;
	else if (opacity > 255) opacity = 255;

	x = Camera::GetInstance()->GetPosition()->x;
	y = Camera::GetInstance()->GetPosition()->y;
	Game::GetInstance()->Draw(NOFLIP, x, y, background, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, opacity);
}

void Transition::FadeIn()
{
	if (!isFinished) return;

	isFinished = false;
	opacity = INVISIBLE;
	type = FADE_IN;
}

void Transition::FadeOut()
{
	if (!isFinished || type == FADE_OUT) return;

	isFinished = false;
	opacity = VISIBLE;
	type = FADE_OUT;
}