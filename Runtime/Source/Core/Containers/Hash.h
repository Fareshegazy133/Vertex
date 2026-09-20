// Vertex

#pragma once

#include <type_traits>
#include "Core/Types/Numeric.h"

namespace VCore
{
	template<typename T>
	uint32 HashValue(const T& Value);
	
	uint32 HashBytes(const void* Data, TSize Size);
	uint32 HashCombine(const uint32 A, const uint32 B);

template<typename T>
struct VHash
{
	static uint32 Hash(const T& Value);
};

template<typename T>
struct VHash<T*>
{
	static uint32 Hash(T* Value);
};

template<>
struct VHash<int8>
{
	static uint32 Hash(const int8 Value);
};

template<>
struct VHash<uint8>
{
	static uint32 Hash(const uint8 Value);
};

template<>
struct VHash<int16>
{
	static uint32 Hash(const int16 Value);
};

template<>
struct VHash<uint16>
{
	static uint32 Hash(const uint16 Value);
};

template<>
struct VHash<int32>
{
	static uint32 Hash(const int32 Value);
};

template<>
struct VHash<uint32>
{
	static uint32 Hash(const uint32 Value);
};

template<>
struct VHash<int64>
{
	static uint32 Hash(const int64 Value);
};

template<>
struct VHash<uint64>
{
	static uint32 Hash(const uint64 Value);
};
}

#include "Core/Containers/Hash.inl"