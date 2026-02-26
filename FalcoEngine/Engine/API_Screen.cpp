#include "stdafx.h"
#include "API_Screen.h"
#include "Screen.h"

int API_Screen::getWidth()
{
	return Screen::GetScreenSize().x;
}

int API_Screen::getHeight()
{
	return Screen::GetScreenSize().y;
}
