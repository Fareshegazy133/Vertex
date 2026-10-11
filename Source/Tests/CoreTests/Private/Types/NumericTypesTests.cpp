// Copyright HNDRED GAMES. All Rights Reserved.

#include "Core/Types/NumericTypes.h"
#include <cstddef>
#include <cstdint>
#include <limits>
#include <type_traits>

static_assert(std::is_same_v<int8, std::int8_t>);
static_assert(std::is_same_v<int16, std::int16_t>);
static_assert(std::is_same_v<int32, std::int32_t>);
static_assert(std::is_same_v<int64, std::int64_t>);
static_assert(std::is_same_v<uint8, std::uint8_t>);
static_assert(std::is_same_v<uint16, std::uint16_t>);
static_assert(std::is_same_v<uint32, std::uint32_t>);
static_assert(std::is_same_v<uint64, std::uint64_t>);

static_assert(std::is_same_v<usize, std::size_t>);
static_assert(std::is_same_v<intptr, std::intptr_t>);
static_assert(std::is_same_v<uintptr, std::uintptr_t>);

static_assert(std::is_same_v<float32, float>);
static_assert(sizeof(float32) == 4);
static_assert(std::numeric_limits<float32>::is_iec559);

static_assert(std::is_same_v<float64, double>);
static_assert(sizeof(float64) == 8);
static_assert(std::numeric_limits<float64>::is_iec559);

static_assert(Vertex::MinInt8 == std::numeric_limits<int8>::min());
static_assert(Vertex::MinInt16 == std::numeric_limits<int16>::min());
static_assert(Vertex::MinInt32 == std::numeric_limits<int32>::min());
static_assert(Vertex::MinInt64 == std::numeric_limits<int64>::min());
static_assert(Vertex::MaxInt8 == std::numeric_limits<int8>::max());
static_assert(Vertex::MaxInt16 == std::numeric_limits<int16>::max());
static_assert(Vertex::MaxInt32 == std::numeric_limits<int32>::max());
static_assert(Vertex::MaxInt64 == std::numeric_limits<int64>::max());

static_assert(Vertex::MinUInt8 == std::numeric_limits<uint8>::min());
static_assert(Vertex::MinUInt16 == std::numeric_limits<uint16>::min());
static_assert(Vertex::MinUInt32 == std::numeric_limits<uint32>::min());
static_assert(Vertex::MinUInt64 == std::numeric_limits<uint64>::min());
static_assert(Vertex::MaxUInt8 == std::numeric_limits<uint8>::max());
static_assert(Vertex::MaxUInt16 == std::numeric_limits<uint16>::max());
static_assert(Vertex::MaxUInt32 == std::numeric_limits<uint32>::max());
static_assert(Vertex::MaxUInt64 == std::numeric_limits<uint64>::max());
