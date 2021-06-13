#include <d3dx9.h>

#include "debug.h"
#include "Game.h"
#include "GameObject.h"

/*
	Initialize game object and load a texture
*/
GameObject::GameObject(float x, float y, LPDIRECT3DTEXTURE9 tex)
{
	this->x = x;
	this->y = y;
	this->texture = tex;
}

void GameObject::Render()
{
	Game::GetInstance()->Draw(x, y, texture);
}

GameObject::~GameObject()
{
	if (texture != NULL) texture->Release();
}

#define MARIO_VX 0.1f
#define MARIO_WIDTH 20