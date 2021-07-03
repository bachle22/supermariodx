#include <string>

#include "Game.h"
#include "HUD.h"
#include "Definition.h"
#include "Debug.h"

HUD::HUD()
{
	world = 1; lives = 0;
	offset_y = SCREEN_HEIGHT - HUD_Y_TRANSFROM - HUD_BAR_HEIGHT;
	statsHUD = Sprites::GetInstance()->Get(STATS);
	powerupHUD = Sprites::GetInstance()->Get(POWERUP);
	icon = Sprites::GetInstance()->Get(ICON_MARIO);
	background = Textures::GetInstance()->Get(ID_TEX_BACKGROUND);
	powerEmpty = Sprites::GetInstance()->Get(POWER_EMPTY);
	arrowEmpty = Sprites::GetInstance()->Get(ARROW_EMPTY);
	font = new Font();
	SetScore(888);
	SetMoney(3);
	SetTime(300);
}

void HUD::Render()
{
	Game* game = Game::GetInstance();
	Game::GetInstance()->GetCamPos(x, y);

	Game::GetInstance()->Draw(1, x, y + offset_y, background, 0, 0, SCREEN_WIDTH, HUD_BAR_HEIGHT, 255);
	statsHUD->Draw(-1, x + STATS_OFFSET_X, y + offset_y + MARGIN, 255);
	powerupHUD->Draw(-1, x + HUD_SMALL_X, y + offset_y + MARGIN, 255);

	for (int i = 0; i < 6; i++)
		arrowEmpty->Draw(-1, x + ARROW_X + FONT_SIZE * i, y + offset_y + STATS_Y, 255);
	powerEmpty->Draw(-1, x + POWER_X, y + offset_y + STATS_Y, 255);

	font->Map('0' + world)->Draw(-1, x + STAGE_X, y + offset_y + STATS_Y, 255);
	font->Map('0' + lives)->Draw(-1, x + STAGE_X, y + offset_y + LIVES_Y, 255);
	icon->Draw(-1, x + ICON_X, y + offset_y + LIVES_Y, 255);

	// Render score
	for (int i = 0; i < SCORE_LENGTH; i++)
		score[i]->Draw(-1, x + ARROW_X + FONT_SIZE * i, y + offset_y + LIVES_Y, 255);

	// Render money
	money[0]->Draw(-1, x + MONEY_X, y + offset_y + STATS_Y, 255);
	if (money.size() > 1) money[1]->Draw(-1, x + MONEY_X - FONT_SIZE, y + offset_y + STATS_Y, 255);

	// Render time
	for (int i = 0; i < TIME_LENGTH; i++)
		time[i]->Draw(-1, x + TIME_X + FONT_SIZE * i, y + offset_y + TIME_Y, 255);
}

void HUD::SetScore(int s)
{
	std::string str = std::to_string(s);
	str.insert(str.begin(), SCORE_LENGTH - str.size(), '0'); // Padding
	score = font->StringToSprites(str);
}

void HUD::SetMoney(int s)
{
	std::string str = std::to_string(s);
	std::string reversed;
	for (int i = str.size() - 1; i >= 0; i--) reversed = reversed.append(1, str[i]);
	money = font->StringToSprites(reversed);
}

void HUD::SetTime(int s)
{
	std::string str = std::to_string(s);
	str.insert(str.begin(), TIME_LENGTH - str.size(), '0'); // Padding
	time = font->StringToSprites(str);
}