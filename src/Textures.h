#pragma once
#include <unordered_map>
#include <d3dx9.h>

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