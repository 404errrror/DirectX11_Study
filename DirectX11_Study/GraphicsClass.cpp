#include "stdafx.h"
#include "GraphicsClass.h"


GraphicsClass::GraphicsClass()
{
}


GraphicsClass::GraphicsClass(const GraphicsClass&)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	return true;
}


void GraphicsClass::ShutDown()
{
}


bool GraphicsClass::Frame()
{
	return true;
}


bool GraphicsClass::Render()
{
	return true;
}