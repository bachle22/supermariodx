#pragma once

#include <d3d9.h>

#include "GameObject.h"
#include "Sprites.h"
#include "Textures.h"

class TileMap /*: public GameObject*/
{
	int mapRows, mapColumns;
	int tilesheetRows, tilesheetColumns;
	int totalTiles;

	LPDIRECT3DTEXTURE9 tileTexture; 
	std::vector<LPSPRITE> tileSprites;
	int** tiles;

public:
	TileMap(int textureId,
		int mapRows, int mapColumns,
		int tilesheetRows, int tilesheetColumns,
		int totalTiles);
	~TileMap();
	virtual void Render();
	//virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom);
	void SetTileMapData(int** tiles);
	void SetTileSprites();
};

