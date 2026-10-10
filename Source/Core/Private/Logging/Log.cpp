// Copyright HNDRED GAMES. All Rights Reserved.

#include "Core/Logging/Log.h"
#include "Logging/FormatToBuffer.h"
#include <array>
#include <cstddef>
#include <cstdio>

namespace
{
	constexpr std::size_t MaxLogLineLength = 1024;

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

void Vertex::Private::WriteLog(const ELogLevel LogLevel, const std::string_view Format, const std::format_args Arguments)
{
	std::array<char, MaxLogLineLength> Buffer;
	std::FILE* Stream = LogLevelToStream(LogLevel);

	const std::string_view Message = FormatToBuffer(Buffer, Format, Arguments);
	static_cast<void>(std::fprintf(Stream, "[%s] %.*s\n", LogLevelToString(LogLevel), static_cast<int>(Message.size()), Message.data()));
	static_cast<void>(std::fflush(Stream));
}
