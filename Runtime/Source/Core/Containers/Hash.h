// Vertex

#pragma once

#include "Core/Types/Numeric.h"
#include "Core/Types/String.h"

namespace VCore
{
using VHashType = uint64;
	
namespace VHashFunctions
{
	VHashType HashBytes(const void* Data, const uint64 Size);
	VHashType Combine(VHashType A, VHashType B);
}

template <typename KeyType>
struct VHash
{
	VHashType operator()(const KeyType& Key) const;
};

template <>
struct VHash<int32>
{
	VHashType operator()(int32 Key) const;
};

template <>
struct VHash<uint32>
{
	VHashType operator()(uint32 Key) const;
};

template <>
struct VHash<int64>
{
	VHashType operator()(int64 Key) const;
};

template <>
struct VHash<uint64>
{
	VHashType operator()(uint64 Key) const;
};

template <>
struct VHash<float32>
{
	VHashType operator()(float32 Key) const;
};

template <>
struct VHash<float64>
{
	VHashType operator()(float64 Key) const;
};

template <>
struct VHash<bool>
{
	VHashType operator()(bool Key) const;
};

template <>
struct VHash<char>
{
	VHashType operator()(char Key) const;
};

template <>
struct VHash<VString>
{
	VHashType operator()(const VString& Key) const;
};
}

#include "Core/Containers/Hash.inl"