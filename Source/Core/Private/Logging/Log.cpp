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

		/**
		 * Only a value cast in from outside the enum gets here. MSVC still requires a
		 * return after the switch (C4715).
		 */
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

		/** Unreachable for valid levels; required for the same reason as above. */
		return stdout;
	}
}

void Vertex::Log(const ELogLevel LogLevel, const std::string_view Message)
{
	std::FILE* Stream = LogLevelToStream(LogLevel);

	/**
	 * One call per line: the C runtime locks the stream for the whole call, so lines
	 * from different threads can't interleave. %.*s stops after Message.size()
	 * characters instead of looking for a null terminator, which a string_view lacks.
	 * The return values are ignored on purpose: if the console write fails, there is
	 * nowhere left to report it, and logging must never take the engine down.
	 */
	static_cast<void>(std::fprintf(Stream, "[%s] %.*s\n", LogLevelToString(LogLevel), static_cast<int>(Message.size()), Message.data()));

	/**
	 * stdout is fully buffered when redirected to a file or pipe; stderr never is.
	 * Without this flush, Warning and Error lines overtake earlier Info lines, and
	 * lines still sitting in the buffer are lost if the process crashes.
	 */
	static_cast<void>(std::fflush(Stream));
}
