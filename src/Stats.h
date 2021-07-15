#pragma once

class Stats
{
	static Stats* __instance;

	int score;
	int coins;
	int lives;
	int cards[3] = { 0 };
public:
	Stats();
	static Stats* GetInstance();

	void AddPoint(int point);
	void AddCoin();

	int GetScore() { return score; }
	int GetCoins() { return coins; }
	int GetLives() { return lives; }

	void AddLife() { lives++; }
	void LoseLife() { lives--; }
};