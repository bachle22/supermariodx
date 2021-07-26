#include "Scene.h"
#include "Transition.h"

Scene::Scene(int id, LPCWSTR filePath)
{
	this->id = id;
	this->sceneFilePath = filePath;
	this->keyHandler = NULL;
	isLoaded = false;
}

Scene::~Scene()
{
	delete sceneFilePath, keyHandler;
}