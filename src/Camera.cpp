#include "Camera.h"
#include "ScenePlayer.h"
#include "Debug.h"
#include "TileMap.h"

Camera* Camera::__instance = NULL;

Camera::Camera()
{
	position = D3DXVECTOR2(0, 0);
	width = Game::GetInstance()->GetScreenWidth();
	height = Game::GetInstance()->GetScreenHeight();

	viewLeft = 0; viewTop = 0; 
	viewRight = 0; viewBottom = 0;
}

Camera* Camera::GetInstance()
{
	if (__instance == NULL) __instance = new Camera();
	return __instance;
}

void Camera::SetPosition(D3DXVECTOR2 position)
{
	this->position = position;
}

void Camera::SetPosition(float x, float y)
{
	SetPosition(D3DXVECTOR2(x, y));
}

void Camera::SetViewSize(float left, float top, float right, float bottom)
{
	viewLeft = left;
	viewTop = top;
	viewRight = right;
	viewBottom = bottom;
}

void Camera::SetViewSize(float width, float height)
{
	viewLeft = 0;
	viewTop = 0;
	viewRight = width;
	viewBottom = height;
}

void Camera::Update()
{
	float cx, cy;
	if (viewRight == MOVING_MAP_WIDTH) {
		if (viewLeft < MOVING_MAP_WIDTH - SCREEN_WIDTH + 16) viewLeft += 0.5f;
		cx = 0; cy = 0;
	}
	Mario* mario = ((ScenePlayer*)Game::GetInstance()->GetCurrentScene())->GetPlayer();
	if (mario != NULL)
	{
		if (viewRight != MOVING_MAP_WIDTH) mario->GetPosition(cx, cy);
	}
	

	TileMap* map = ((ScenePlayer*)Game::GetInstance()->GetCurrentScene())->GetMap();
	int w, h;
	if (map != NULL)
	{
		map->GetMapSize(w, h);
		viewRight = (float)w;
		viewBottom = (float)h;
	}
	else
	{
		viewRight = 0;
		viewBottom = 0;
	}

	cx -= (width + 1) / 2; cy -= (height + 1) / 2;
	if (viewRight == 0);
	else if (cx < viewLeft) cx = viewLeft;
	else if (cx > viewRight) cx = viewRight;

	if (cy < CAMERA_CEIL_Y)
		cy = viewBottom - CAMERA_CEIL_Y + cy;
	else cy = viewBottom;
	if (cy < viewTop) cy = 0;

	if (viewRight == 0 && viewBottom == 0)
	{
		cx = 0;
		cy = 0;
	}
	SetPosition(ceil(cx) + Game::GetInstance()->DEBUG_X*2, ceil(cy) + Game::GetInstance()->DEBUG_Y*2);
}

void Camera::GetViewSize(float& viewLeft, float& viewTop, float& viewRight, float& viewBottom)
{
	viewLeft = this->viewLeft;
	viewTop = this->viewTop;
	viewRight = this->viewRight;
	viewBottom = this->viewBottom;
}