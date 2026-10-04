// Copyright HNDRED GAMES. All Rights Reserved.

/** Core logging: the severity levels and the function that writes one line to the console. */

#pragma once

#include <cstdint>
#include <string_view>

/** How severe a log line is. It picks the line's prefix and the stream it goes to. */
enum class ELogLevel : std::uint8_t
{
	/** Normal operation worth recording: startup, shutdown, state changes. */
	Info,
	/** Something unexpected happened, and the engine recovered. */
	Warning,
	/** An operation failed, and its result is missing or wrong. */
	Error
};

namespace Vertex
{
	/**
	 * Writes one line, formatted as "[Level] Message", and adds the newline.
	 *
	 * @param LogLevel Severity. Info goes to stdout; Warning and Error go to stderr.
	 * @param Message The text to write. Read only during the call; it needs no null terminator.
	 * @note Safe to call from any thread: each line is written whole, then flushed.
	 * @note Every call flushes, which costs a write to the OS. Keep logging out of per-frame code.
	 */
	CORE_API void Log(const ELogLevel LogLevel, const std::string_view Message);
}
