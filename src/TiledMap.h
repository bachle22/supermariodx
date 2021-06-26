#pragma once

#include <d3d9.h>

#include "Sprites.h"
#include "Textures.h"
#include "Matrix.h"

constexpr int TILE_WIDTH = 16;
constexpr int TILE_HEIGHT = 16;

class TiledMap
{
	int mapRows, mapColumns;
	int tilesheetRows, tilesheetColumns;
	int totalTiles;

	LPDIRECT3DTEXTURE9 tileTexture; 
	std::vector<LPSPRITE> tileSprites;
	int** tiles;

public:
	TiledMap(int textureId,
		int mapRows, int mapColumns,
		int tilesheetRows, int tilesheetColumns,
		int totalTiles);
	~TiledMap();
	void Render();
	void SetTileMapData(int** tiles);
	void SetTileSprites();
};

