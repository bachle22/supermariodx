#include "Game.h"
#include "HUD.h"
#include "Definition.h"

HUD::HUD()
{
	offset_y = SCREEN_HEIGHT - HUD_Y_TRANSFROM - HUD_BAR_HEIGHT;
	statsHUD = Sprites::GetInstance()->Get(STATS);
	powerupHUD = Sprites::GetInstance()->Get(POWERUP);
}

void HUD::Render()
{
	if (!background) background = Textures::GetInstance()->Get(ID_TEX_BACKGROUND);
	Game::GetInstance()->GetCamPos(x, y);
	Game::GetInstance()->Draw(1, x, y + offset_y, background, 0, 0, SCREEN_WIDTH, HUD_BAR_HEIGHT, 255);
	Sprites::GetInstance()->Get(STATS)->Draw(-1, x + 10, y + offset_y + 2, 255);
	Sprites::GetInstance()->Get(POWERUP)->Draw(-1, x + HUD_SMALL_X, y + offset_y + 2, 255);
}