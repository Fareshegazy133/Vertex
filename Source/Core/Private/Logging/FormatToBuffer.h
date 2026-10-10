// Copyright HNDRED GAMES. All Rights Reserved.

/** Bounded formatting for Core: text goes into a buffer the caller owns, never onto the heap. Logging and asserts both build on it. */

#pragma once

#include <format>
#include <span>
#include <string_view>

namespace Vertex
{
	/**
	 * Formats text into a caller-owned buffer, and cuts it off when the buffer is full.
	 *
	 * @param Buffer Where the text is written. Its size is the most characters the result can hold.
	 * @param Format A std::format string, such as the one a std::format_string returns from get().
	 * @param Arguments The values to format, packed by std::make_format_args. Read only during the call.
	 * @return The text written: a view into Buffer, shorter than the full text when it was cut. Valid as long as Buffer is.
	 * @warning Format is checked at run time here: an invalid one throws std::format_error. Pass the get() of a std::format_string, which the compiler has already checked.
	 * @note The result needs no heap memory, and has no null terminator.
	 * @note Text is cut at a byte, so a multibyte UTF-8 character at the edge can be split in half.
	 */
	std::string_view FormatToBuffer(const std::span<char> Buffer, const std::string_view Format, const std::format_args Arguments);
}
