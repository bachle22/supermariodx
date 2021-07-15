#include "Sprites.h"
#include "Game.h"
#include "Debug.h"

Sprite::Sprite(int id,
	int left,
	int top,
	int right,
	int bottom,
	LPDIRECT3DTEXTURE9 tex)
{
	this->id = id;
	this->left = left;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
	this->texture = tex;
}

Sprites* Sprites::__instance = NULL;

Sprites* Sprites::GetInstance()
{
	if (__instance == NULL) __instance = new Sprites();
	return __instance;
}

void Sprite::Draw(int nx, float x, float y, int alpha)
{
	// if (id == DISABLED_SPRITE_ID) return;
	Game* game = Game::GetInstance();
	game->Draw(nx, x, y, texture, left, top, right, bottom, alpha);
}

void Sprite::DrawClippedSprite(int nx, float x, float y, int alpha, int width, int height)
{
	// if (id == DISABLED_SPRITE_ID) return;
	Game* game = Game::GetInstance();
	game->Draw(nx, x, y, texture, left, top, left + width, top + height, alpha);
}

void Sprites::Add(int id, int left, int top, int right, int bottom, LPDIRECT3DTEXTURE9 tex)
{
	LPSPRITE s = new Sprite(id, left, top, right, bottom, tex);
	sprites[id] = s;

}

LPSPRITE Sprites::Get(int id)
{
	return sprites[id];
}

/*
	Clear all loaded sprites
*/
void Sprites::Clear()
{
	for (auto x : sprites)
	{
		LPSPRITE s = x.second;
		delete s;
	}

	sprites.clear();
}
