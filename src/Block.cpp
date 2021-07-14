#include "Block.h"

Block::Block(float width, float height)
{
	this->width = width;
	this->height = height;
}

void Block::Render()
{
	RenderBoundingBox();
}

void Block::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	left = x;
	top = y;
	right = x + width;
	bottom = y + height;
}