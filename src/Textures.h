#pragma once
#include <unordered_map>

enum TexturesID
{
	ID_TEX_BBOX = -100,	// special texture to draw object bounding box
	ID_TEX_BACKGROUND = -1,
};

/*
	Manage texture database
*/
class Textures
{
	static Textures* __instance;
	std::unordered_map<int, LPDIRECT3DTEXTURE9> textures;

public:
	Textures() noexcept {};
	void Add(int id, LPCWSTR filePath, D3DCOLOR transparentColor);
	LPDIRECT3DTEXTURE9 Get(int i);

	void Clear();
	static Textures* GetInstance();
};