// Copyright HNDRED GAMES. All Rights Reserved.

/**
 * The numeric vocabulary for all of Vertex: one alias per width, used instead of int, long, float, and std::size_t,
 * plus the integer limits that go with them. The aliases are global and lowercase, like the built-in types they stand for.
 */

#pragma once

#include <cstddef>
#include <cstdint>

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

using usize = std::size_t;
using intptr = std::intptr_t;
using uintptr = std::uintptr_t;

using float32 = float;
using float64 = double;

namespace Vertex
{
	/** The most negative int8, -128. */
	inline constexpr int8 MinInt8 = INT8_MIN;

	/** The most negative int16, -32768. */
	inline constexpr int16 MinInt16 = INT16_MIN;

	/** The most negative int32, -2147483648. Negating it overflows int32, which is undefined behavior. */
	inline constexpr int32 MinInt32 = INT32_MIN;

	/** The most negative int64, -9223372036854775808. Negating it overflows int64, which is undefined behavior. */
	inline constexpr int64 MinInt64 = INT64_MIN;

	/** The largest int8, 127. */
	inline constexpr int8 MaxInt8 = INT8_MAX;

	/** The largest int16, 32767. */
	inline constexpr int16 MaxInt16 = INT16_MAX;

	/** The largest int32, 2147483647. */
	inline constexpr int32 MaxInt32 = INT32_MAX;

	/** The largest int64, 9223372036854775807. */
	inline constexpr int64 MaxInt64 = INT64_MAX;

	/** The smallest uint8, 0. Every unsigned type starts at 0; this names it so each type has a Min and a Max. */
	inline constexpr uint8 MinUInt8 = 0;

	/** The smallest uint16, 0. Every unsigned type starts at 0; this names it so each type has a Min and a Max. */
	inline constexpr uint16 MinUInt16 = 0;

	/** The smallest uint32, 0. Every unsigned type starts at 0; this names it so each type has a Min and a Max. */
	inline constexpr uint32 MinUInt32 = 0;

	/** The smallest uint64, 0. Every unsigned type starts at 0; this names it so each type has a Min and a Max. */
	inline constexpr uint64 MinUInt64 = 0;

	/** The largest uint8, 255. */
	inline constexpr uint8 MaxUInt8 = UINT8_MAX;

	/** The largest uint16, 65535. */
	inline constexpr uint16 MaxUInt16 = UINT16_MAX;

	/** The largest uint32, 4294967295. */
	inline constexpr uint32 MaxUInt32 = UINT32_MAX;

	/** The largest uint64, 18446744073709551615. */
	inline constexpr uint64 MaxUInt64 = UINT64_MAX;
}
