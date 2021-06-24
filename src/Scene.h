#pragma once

#include <d3dx9.h>
#include "Input.h"

class Scene
{
protected:
	int id;
	LPCWSTR sceneFilePath;
	LPKEYEVENTHANDLER keyHandler;

public:
	Scene(int id, LPCWSTR filePath);

	InputHandler* GetKeyEventHandler() { return keyHandler; }
	virtual void Load() = 0;
	virtual void Unload() = 0;
	virtual void Update(ULONGLONG dt) = 0;
	virtual void Render() = 0;
};

typedef Scene* LPSCENE;


class SceneInputHandler : public InputHandler
{
protected:
	LPSCENE scene;

public:
	virtual void KeyState(BYTE* states) = 0;
	virtual void OnKeyDown(int KeyCode) = 0;
	virtual void OnKeyUp(int KeyCode) = 0;
	SceneInputHandler(LPSCENE s) : InputHandler() { scene = s; }
};