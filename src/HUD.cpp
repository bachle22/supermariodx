#include <string>

#include "Game.h"
#include "Camera.h"
#include "HUD.h"
#include "Definition.h"
#include "Debug.h"
#include "Mario.h"
#include "ScenePlayer.h"
#include "Stats.h"

HUD::HUD()
{
	// Get sprites
	statsHUD = Sprites::GetInstance()->Get(STATS);
	powerupHUD = Sprites::GetInstance()->Get(POWERUP);
	icon = Sprites::GetInstance()->Get(ICON_MARIO);
	background = Textures::GetInstance()->Get(ID_TEX_BACKGROUND);
	powerEmpty = Sprites::GetInstance()->Get(POWER_EMPTY);
	powerFilled = Sprites::GetInstance()->Get(POWER_FILLED);
	arrowEmpty = Sprites::GetInstance()->Get(ARROW_EMPTY);
	arrowFilled = Sprites::GetInstance()->Get(ARROW_FILLED);
	powerMeterIcon = AnimationSets::GetInstance()->Get(POWER_ANI)->at(0);

	// Set default HUD values
	font = new Font();
	world = 1;
	SetScore();
	SetCoins();
	SetTime(0);
	SetPowerMeter(0);
	offset_y = SCREEN_HEIGHT - HUD_Y_TRANSFROM - HUD_BAR_HEIGHT;
}

void HUD::Render()
{
	Game* game = Game::GetInstance();
	x = Camera::GetInstance()->GetPosition().x;
	y = Camera::GetInstance()->GetPosition().y;
	x = floor(x); y = floor(y);
	// Render HUD containers textures
	Game::GetInstance()->Draw(x, y + offset_y, background, 0, 0, SCREEN_WIDTH, HUD_BAR_HEIGHT);
	statsHUD->Draw(x + STATS_OFFSET_X, y + offset_y + MARGIN);
	powerupHUD->Draw(x + HUD_SMALL_X, y + offset_y + MARGIN);

	// Render power meter
	for (int i = powerMeter; i < MAX_POWER_METER - 1; i++)
		arrowEmpty->Draw(x + ARROW_X + FONT_SIZE * i, y + offset_y + STATS_Y);
	if (powerMeter > 0) for (int i = 0; i < powerMeter && i < MAX_POWER_METER - 1; i++)
		arrowFilled->Draw(x + ARROW_X + FONT_SIZE * i, y + offset_y + STATS_Y);

	if (powerMeter < MAX_POWER_METER)
		powerEmpty->Draw(x + POWER_X, y + offset_y + STATS_Y);
	else powerMeterIcon->Render(x + POWER_X, y + offset_y + STATS_Y);

	font->Map('0' + world)->Draw(x + STAGE_X, y + offset_y + STATS_Y);
	font->Map('0' + Stats::GetInstance()->GetLives())->
		Draw(x + STAGE_X, y + offset_y + LIVES_Y);
	icon->Draw(x + ICON_X, y + offset_y + LIVES_Y);

	// Render score
	for (int i = 0; i < SCORE_LENGTH; i++)
		score[i]->Draw(x + ARROW_X + FONT_SIZE * i, y + offset_y + LIVES_Y);

	// Render money
	money[0]->Draw(x + MONEY_X, y + offset_y + STATS_Y);
	if (money.size() > 1) money[1]->Draw(x + MONEY_X - FONT_SIZE, y + offset_y + STATS_Y);

	// Render time
	for (int i = 0; i < TIME_LENGTH; i++)
		time[i]->Draw(x + TIME_X + FONT_SIZE * i, y + offset_y + TIME_Y);
}

void HUD::SetScore()
{
	int s = Stats::GetInstance()->GetScore();
	if (s > SCORE_MAX) s = 0;
	std::string str = std::to_string(s);
	str.insert(str.begin(), SCORE_LENGTH - str.size(), '0'); // Padding
	score = font->StringToSprites(str);
}

void HUD::SetCoins()
{
	int c = Stats::GetInstance()->GetCoins();
	std::string str = std::to_string(c);
	std::string reversed;
	for (int i = str.size() - 1; i >= 0; i--) reversed = reversed.append(1, str[i]);
	money = font->StringToSprites(reversed);
}

void HUD::SetTime(int s)
{
	if (s > TIME_MAX) s = 0;
	std::string str = std::to_string(s);
	str.insert(str.begin(), TIME_LENGTH - str.size(), '0'); // Padding
	time = font->StringToSprites(str);
}

void HUD::SetPowerMeter(int p)
{
	powerMeter = p;
}

void HUD::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	return;
}