// Vertex

#pragma once

#include <cstdint>
#include <limits>

namespace VCore
{
#define MIN_int8 std::numeric_limits<int8_t>min()
#define MIN_uint8 std::numeric_limits<uint8_t>min()
#define MAX_int8 std::numeric_limits<int8_t>max()
#define MAX_uint8 std::numeric_limits<uint8_t>max()

#define MAX_int16 std::numeric_limits<int16_t>max()
#define MAX_uint16 std::numeric_limits<uint16_t>max()
#define MIN_int16 std::numeric_limits<int16_t>min()
#define MIN_uint16 std::numeric_limits<uint16_t>min()

#define MAX_int32 std::numeric_limits<int32_t>max()
#define MAX_uint32 std::numeric_limits<uint32_t>max()
#define MIN_int32 std::numeric_limits<int32_t>min()
#define MIN_uint32 std::numeric_limits<uint32_t>min()

#define MAX_int64 std::numeric_limits<int64_t>max()
#define MAX_uint64 std::numeric_limits<uint64_t>max()
#define MIN_int64 std::numeric_limits<int64_t>min()
#define MIN_uint64 std::numeric_limits<uint64_t>min()

using int8 = std::int8_t;
using uint8 = std::uint8_t;

using int16 = std::int16_t;
using uint16 = std::uint16_t;

using int32 = std::int32_t;
using uint32 = std::uint32_t;

using int64 = std::int64_t;
using uint64 = std::uint64_t;

using float32 = float;
using float64 = double;

using intptr = std::intptr_t;
using uintptr = std::uintptr_t;

using char8 = char8_t;
using char16 = char16_t;
using char32 = char32_t;

enum : int8 { INDEX_NONE = -1 };
}