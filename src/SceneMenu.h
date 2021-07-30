#pragma once

#include "Scene.h"
#include "GameObject.h"
#include "HUD.h"
#include "TileMap.h"
#include "MenuPlayer.h"

class SceneMenu : public Scene
{
	static SceneMenu* __instance;
	friend class SceneMenuInputHandler;

protected:
	MenuPlayer* player{};

	ULONGLONG interval{};
	int timer{};
	int score{};

	HUD* hud{};
	TileMap* map{};
	std::vector<LPGAMEOBJECT> objects;

	bool isTransitionDone{};


	void _ParseSection_TEXTURES(std::string line);
	void _ParseSection_SPRITES(std::string line);
	void _ParseSection_ANIMATIONS(std::string line);
	void _ParseSection_ANIMATION_SETS(std::string line);
	void _ParseSection_OBJECTS(std::string line);
	void _ParseSection_TILEMAP(std::string line);

public:
	SceneMenu(int id, LPCWSTR filePath);

	void Init();
	virtual void Load();
	virtual void Update(ULONGLONG dt);
	virtual void Render();
	virtual void Unload();

	MenuPlayer* GetPlayer() { return player; }
};

class SceneMenuInputHandler : public SceneInputHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode);
	SceneMenuInputHandler(LPSCENE s) : SceneInputHandler(s) {};
	bool isCameraFollowing = false;

};

