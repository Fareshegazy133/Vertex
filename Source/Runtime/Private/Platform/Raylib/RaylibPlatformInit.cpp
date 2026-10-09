// Copyright HNDRED GAMES. All Rights Reserved.

#include "Platform/PlatformInit.h"
#include "Core/Logging/Log.h"
#include "raylib.h"

void Vertex::InitializePlatform()
{
	SetTraceLogLevel(LOG_WARNING);
	Log(ELogLevel::Info, "Platform initialized: raylib " RAYLIB_VERSION);
}
