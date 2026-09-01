// Vertex

#include "Core/Application/Window.h"

#include "raylib.h"

namespace VCore
{
VWindow::VWindow(const VWindowSpecification& InWindowSpecification)
	: WindowSpecification(InWindowSpecification)
{}

VWindow::~VWindow()
{
	Destruct();
}

void VWindow::Construct() const
{
	uint8 ConfigFlags = 0;

	if (WindowSpecification.bVSync)
	{
		ConfigFlags |= FLAG_VSYNC_HINT;
	}

	if (WindowSpecification.bResizable)
	{
		ConfigFlags |= FLAG_WINDOW_RESIZABLE;
	}
	
	SetConfigFlags(ConfigFlags);
	InitWindow(WindowSpecification.Width, WindowSpecification.Height, WindowSpecification.WindowName);
}

void VWindow::Destruct() const
{
	CloseWindow();
}
}