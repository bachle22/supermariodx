#include "windows.h"
#include "Stats.h"
#include "HUD.h"
#include "ScenePlayer.h"
#include "Game.h"

Stats* Stats::__instance = NULL;

Stats* Stats::GetInstance()
{
	if (__instance == NULL) __instance = new Stats();
	return __instance;
}

Stats::Stats()
{
	score = 0;
	coins = 0;
	lives = 4;
}


void Stats::AddPoint(int point)
{
	score += point;
	LPSCENE scene = Game::GetInstance()->GetCurrentScene();
	HUD* hud = ((ScenePlayer*)scene)->GetHUD();
	if (hud != NULL) hud->SetScore();
}

void Stats::AddCoin()
{
	coins++;
	LPSCENE scene = Game::GetInstance()->GetCurrentScene();
	HUD* hud = ((ScenePlayer*)scene)->GetHUD();
	if (hud != NULL) hud->SetCoins();
}