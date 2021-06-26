#include "Game.h"
#include "TiledMap.h"

TiledMap::TiledMap(int textureId, int mapRows, int mapColumns, int tilesheetRows, int  tilesheetColumns, int totalTiles)
{
	tileTexture = Textures::GetInstance()->Get(textureId);
	this->mapRows = mapRows;
	this->mapColumns = mapColumns;
	this->tilesheetRows = tilesheetRows;
	this->tilesheetColumns = tilesheetColumns;
	this->totalTiles = totalTiles;
}

TiledMap::~TiledMap()
{
	delete tileTexture, tileSprites, tiles;
}

void TiledMap::Render()
{
	for (int i = 0; i < mapRows; i++)
		for (int j = 0; j < mapColumns; j++)
			tileSprites[tiles[i][j] - 1]->Draw(j * TILE_WIDTH, i * TILE_HEIGHT, 255);
}

void TiledMap::SetTileMapData(int** tiles)
{
	this->tiles = tiles;
}


void TiledMap::SetTileSprites()
{
	for (int i = 0; i < totalTiles; i++)
	{
		int left = i % tilesheetColumns * TILE_WIDTH;
		int top = i / tilesheetColumns * TILE_HEIGHT;
		int right = left + TILE_WIDTH;
		int bottom = top + TILE_HEIGHT;
		LPSPRITE sprite = new Sprite(i, left, top, right, bottom, tileTexture);
		this->tileSprites.push_back(sprite);
	}
}
