#include <string>

#include "Game.h"
#include "Camera.h"
#include "HUD.h"
#include "Definition.h"
#include "Debug.h"
#include "Mario.h"
#include "ScenePlayer.h"

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
	powerMeterIcon = AnimationSets::GetInstance()->Get(77)->at(0);

	// Set default HUD values
	font = new Font();
	world = 1; lives = 0;
	SetScore(0);
	SetMoney(0);
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
	Game::GetInstance()->Draw(NOFLIP, x, y + offset_y, background, 0, 0, SCREEN_WIDTH, HUD_BAR_HEIGHT, OPAQUED);
	statsHUD->Draw(NOFLIP, x + STATS_OFFSET_X, y + offset_y + MARGIN, OPAQUED);
	powerupHUD->Draw(NOFLIP, x + HUD_SMALL_X, y + offset_y + MARGIN, OPAQUED);

	// Render power meter
	for (int i = powerMeter; i < MAX_POWER_METER - 1; i++)
		arrowEmpty->Draw(NOFLIP, x + ARROW_X + FONT_SIZE * i, y + offset_y + STATS_Y, OPAQUED);
	if (powerMeter > 0) for (int i = 0; i < powerMeter && i < MAX_POWER_METER - 1; i++)
		arrowFilled->Draw(NOFLIP, x + ARROW_X + FONT_SIZE * i, y + offset_y + STATS_Y, OPAQUED);

	if (powerMeter < MAX_POWER_METER)
		powerEmpty->Draw(NOFLIP, x + POWER_X, y + offset_y + STATS_Y, OPAQUED);
	else powerMeterIcon->Render(NOFLIP, x + POWER_X, y + offset_y + STATS_Y, OPAQUED);

	font->Map('0' + world)->Draw(NOFLIP, x + STAGE_X, y + offset_y + STATS_Y, OPAQUED);
	font->Map('0' + lives)->Draw(NOFLIP, x + STAGE_X, y + offset_y + LIVES_Y, OPAQUED);
	icon->Draw(NOFLIP, x + ICON_X, y + offset_y + LIVES_Y, OPAQUED);

	// Render score
	for (int i = 0; i < SCORE_LENGTH; i++)
		score[i]->Draw(NOFLIP, x + ARROW_X + FONT_SIZE * i, y + offset_y + LIVES_Y, OPAQUED);

	// Render money
	money[0]->Draw(NOFLIP, x + MONEY_X, y + offset_y + STATS_Y, OPAQUED);
	if (money.size() > 1) money[1]->Draw(NOFLIP, x + MONEY_X - FONT_SIZE, y + offset_y + STATS_Y, OPAQUED);

	// Render time
	for (int i = 0; i < TIME_LENGTH; i++)
		time[i]->Draw(NOFLIP, x + TIME_X + FONT_SIZE * i, y + offset_y + TIME_Y, OPAQUED);
}

void HUD::SetScore(int s)
{
	if (s > SCORE_MAX) s = 0;
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
	if (s > TIME_MAX) s = 0;
	std::string str = std::to_string(s);
	str.insert(str.begin(), TIME_LENGTH - str.size(), '0'); // Padding
	time = font->StringToSprites(str);
}

void HUD::SetPowerMeter(int p)
{
	powerMeter = p;
}