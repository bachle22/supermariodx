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
	Mario* mario = ((ScenePlayer*)Game::GetInstance()->GetCurrentScene())->GetPlayer();
	if (mario == NULL) return;
	mario->GetPosition(cx, cy);

	TileMap* map = ((ScenePlayer*)Game::GetInstance()->GetCurrentScene())->GetMap();
	if (map != NULL) map->GetMapSize(viewRight, viewBottom);

	cx -= (width + 1) / 2; cy -= (height + 1) / 2;
	if (viewRight == 0);
	else if (cx < viewLeft) cx = 0;
	else if (cx > viewRight) cx = viewRight;

	// TODO: Only follow Mario vertical position when on the cloud
	if (cy < CAMERA_CEIL_Y)
		cy = viewBottom - CAMERA_CEIL_Y + cy;
	else cy = viewBottom;
	if (cy < viewTop) cy = 0;

	SetPosition(ceil(cx) + Game::GetInstance()->DEBUG_X*2, ceil(cy) + Game::GetInstance()->DEBUG_Y*2);
}
