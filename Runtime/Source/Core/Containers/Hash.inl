// Vertex

#pragma once

namespace VCore
{
inline uint32 HashBytes(const void* Data, const TSize Size)
{
	constexpr uint32 OffsetBasis = 2166136261u;
	constexpr uint32 Prime = 16777619u;

	const auto* Bytes = static_cast<const uint8*>(Data);

	uint32 Hash = OffsetBasis;

	for (TSize Index = 0; Index < Size; Index++)
	{
		Hash ^= Bytes[Index];
		Hash *= Prime;
	}

	return Hash;
}
	
inline uint32 HashCombine(const uint32 A, const uint32 B)
{
	return A ^ (B + 0x9e3779b9u + (A << 6) + (A >> 2));
}

template <typename T>
uint32 VHash<T>::Hash(const T& Value)
{
	if constexpr (std::is_enum_v<T>)
	{
		using UnderlyingType = std::underlying_type_t<T>;
		return VHash<UnderlyingType>::Hash(static_cast<UnderlyingType>(Value));
	}
	else
	{
		return Value.GetHash();
	}
}

template <typename T>
uint32 HashValue(const T& Value)
{
	return VHash<T>::Hash(Value);
}

template <typename T>
uint32 VHash<T*>::Hash(T* Value)
{
	return static_cast<uint32>(reinterpret_cast<uintptr_t>(Value));
}

inline uint32 VHash<int8>::Hash(const int8 Value)
{
	return HashBytes(&Value, sizeof(Value));
}

inline uint32 VHash<uint8>::Hash(const uint8 Value)
{
	return HashBytes(&Value, sizeof(Value));
}

inline uint32 VHash<int16>::Hash(const int16 Value)
{
	return HashBytes(&Value, sizeof(Value));
}

inline uint32 VHash<uint16>::Hash(const uint16 Value)
{
	return HashBytes(&Value, sizeof(Value));
}

inline uint32 VHash<int32>::Hash(const int32 Value)
{
	return HashBytes(&Value, sizeof(Value));
}

inline uint32 VHash<uint32>::Hash(const uint32 Value)
{
	return HashBytes(&Value, sizeof(Value));
}

inline uint32 VHash<int64>::Hash(const int64 Value)
{
	return HashBytes(&Value, sizeof(Value));
}

inline uint32 VHash<uint64>::Hash(const uint64 Value)
{
	return HashBytes(&Value, sizeof(Value));
}
}