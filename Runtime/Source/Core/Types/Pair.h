// Vertex

#pragma once

namespace VCore
{
template<typename FirstType, typename SecondType>
struct VPair
{
	VPair() = default;
	VPair(const FirstType& InFirst, const SecondType& InSecond);
	VPair(FirstType&& InFirst, SecondType&& InSecond);
	
	template <typename TFirst, typename TSecond>
	VPair(TFirst&& InFirst, TSecond&& InSecond);
	
	FirstType First;
	SecondType Second;
};
}

#include "Core/Types/Pair.inl"