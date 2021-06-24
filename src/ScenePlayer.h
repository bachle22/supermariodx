#pragma once

#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "GameObject.h"
#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"
#include "Koopa.h"


class ScenePlayer : public Scene
{
protected:
	Mario* player;

	std::vector<LPGAMEOBJECT> objects;

	void _ParseSection_TEXTURES(std::string line);
	void _ParseSection_SPRITES(std::string line);
	void _ParseSection_ANIMATIONS(std::string line);
	void _ParseSection_ANIMATION_SETS(std::string line);
	void _ParseSection_OBJECTS(std::string line);


public:
	ScenePlayer(int id, LPCWSTR filePath);

	virtual void Load();
	virtual void Update(ULONGLONG dt);
	virtual void Render();
	virtual void Unload();

	Mario* GetPlayer() { return player; }

	friend class ScenePlayerInputHandler;
};

class ScenePlayerInputHandler : public SceneInputHandler
{
public:
	virtual void KeyState(BYTE* states);
	virtual void OnKeyDown(int KeyCode);
	virtual void OnKeyUp(int KeyCode) {};
	ScenePlayerInputHandler(LPSCENE s): SceneInputHandler(s) {};
};

