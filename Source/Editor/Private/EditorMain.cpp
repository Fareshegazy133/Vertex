// Copyright HNDRED GAMES. All Rights Reserved.

#include "Core/Logging/Log.h"
#include "Engine/Engine.h"
#include <cstdlib>

int main()
{
	Vertex::InitializeEngine();
	Vertex::Log(ELogLevel::Info, "Editor running. Window arrives in M2");
	return EXIT_SUCCESS;
}
