#include "padding.h"

Padding::Padding(int top, int right, int bottom, int left)
{
	this->top = top;
	this->right = right;
	this->bottom = bottom;
	this->left = left;
	mask = 0;
}

Padding::Padding(int tb, int lr)
{
	top = bottom = tb;
	right = left = lr;
	mask = 0;
}

Padding::Padding(int mask, int top, int right, int bottom, int left)
{
	this->mask = mask;
	this->top = top;
	this->right = right;
	this->bottom = bottom;
	this->left = left;
}

int Padding::getRight(int total)
{
	if (mask & PERPAD_RIGHT)
		return (total * right)/100;
	return right;
}

int Padding::getLeft(int total)
{
	if (mask & PERPAD_LEFT)
		return (total * left)/100;
	return left;
}

int Padding::getTop(int total)
{
	if (mask & PERPAD_TOP)
		return (total * top)/100;
	return top;
}

int Padding::getBottom(int total)
{
	if (mask & PERPAD_BOT)
		return (total * bottom)/100;
	return bottom;
}
