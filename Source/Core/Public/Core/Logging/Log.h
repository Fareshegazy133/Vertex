// Copyright HNDRED GAMES. All Rights Reserved.

/** Core logging: the severity levels, and Log, which formats one line and writes it to the console. */

#pragma once

#include <cstdint>
#include <format>
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
	namespace Private
	{
		/**
		 * Formats one line and writes it. The non-template half of Log, compiled once in Log.cpp.
		 *
		 * @param LogLevel Severity. Info goes to stdout; Warning and Error go to stderr.
		 * @param Format The line's format string, as Log's std::format_string returns it from get().
		 * @param Arguments The values for Format, packed by std::make_format_args.
		 * @warning Call Log instead. This takes a format nobody has checked, and an invalid one throws std::format_error.
		 */
		CORE_API void WriteLog(const ELogLevel LogLevel, const std::string_view Format, const std::format_args Arguments);
	}

	/**
	 * Formats one line, writes it as "[Level] Message", and adds the newline.
	 *
	 * @tparam TArguments The types of the values to format. Deduced from Arguments; never spelled out.
	 * @param LogLevel Severity. Info goes to stdout; Warning and Error go to stderr.
	 * @param Format A std::format string, such as "Loaded {} files". The compiler checks it against Arguments, so a mismatch fails the build.
	 * @param Arguments The values for Format's {} fields. Read only during the call.
	 * @warning Format must be visible to the compiler. To log a run-time string, pass it as an argument: Log(LogLevel, "{}", Text).
	 * @note Safe to call from any thread: each line is written whole, then flushed.
	 * @note Every call flushes, which costs a write to the OS. Keep logging out of per-frame code.
	 * @note Formats into a 1024-character stack buffer, not the heap. Longer lines are cut.
	 */
	template<typename... TArguments>
	void Log(const ELogLevel LogLevel, const std::format_string<TArguments...> Format, TArguments&&... Arguments)
	{
		Private::WriteLog(LogLevel, Format.get(), std::make_format_args(Arguments...));
	}
}
