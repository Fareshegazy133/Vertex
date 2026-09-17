// Vertex

#pragma once

#include "Core/Types/Numeric.h"

namespace VCore
{
enum class EVObjectFlags : uint64
{
	None          = 0,
	Public        = 1ull << 0,
	Transient     = 1ull << 1,
	PendingKill   = 1ull << 2,
	Root          = 1ull << 3,
	DefaultObject = 1ull << 4,
	Native        = 1ull << 5,
	Archetype     = 1ull << 6,
	EditorOnly    = 1ull << 7
};

inline EVObjectFlags operator|(EVObjectFlags A, EVObjectFlags B)
{
	return static_cast<EVObjectFlags>(static_cast<uint64_t>(A) | static_cast<uint64_t>(B));
}

inline EVObjectFlags operator&(EVObjectFlags A, EVObjectFlags B)
{
	return static_cast<EVObjectFlags>(static_cast<uint64_t>(A) & static_cast<uint64_t>(B));
}

inline EVObjectFlags& operator|=(EVObjectFlags& A, EVObjectFlags B)
{
	A = A | B; 
	return A;
}
}