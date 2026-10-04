// Copyright HNDRED GAMES. All Rights Reserved.

#include "Core/Logging/Log.h"
#include <cstdio>

namespace
{
	const char* LogLevelToString(const ELogLevel LogLevel)
	{
		switch (LogLevel)
		{
		case ELogLevel::Info:
			return "Info";
		case ELogLevel::Warning:
			return "Warning";
		case ELogLevel::Error:
			return "Error";
		}

		return "Unknown";
	}

	std::FILE* LogLevelToStream(const ELogLevel LogLevel)
	{
		switch (LogLevel)
		{
		case ELogLevel::Info:
			return stdout;
		case ELogLevel::Warning:
			return stderr;
		case ELogLevel::Error:
			return stderr;
		}

		return stdout;
	}
}

void Vertex::Log(const ELogLevel LogLevel, const std::string_view Message)
{
	std::FILE* Stream = LogLevelToStream(LogLevel);
	static_cast<void>(std::fprintf(Stream, "[%s] %.*s\n", LogLevelToString(LogLevel), static_cast<int>(Message.size()), Message.data()));
	static_cast<void>(std::fflush(Stream));
}
