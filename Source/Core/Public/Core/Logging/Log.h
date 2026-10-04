// Copyright HNDRED GAMES. All Rights Reserved.

#pragma once

#include <cstdint>
#include <string_view>

// How severe a log line is. Switches over it have no default:, so adding a value
// fails the build (C4062) at every switch that doesn't handle it yet.
enum class ELogLevel : std::uint8_t
{
	Info,
	Warning,
	Error
};

namespace Vertex
{
	// Writes "[Level] Message" as one line; the newline is added for you.
	// Info goes to stdout, Warning and Error to stderr.
	// Message is read only during the call and needs no null terminator.
	// Safe to call from any thread: each line is written whole, then flushed. The flush
	// costs a write to the OS on every call, so keep logging out of per-frame code.
	CORE_API void Log(const ELogLevel LogLevel, const std::string_view Message);
}
