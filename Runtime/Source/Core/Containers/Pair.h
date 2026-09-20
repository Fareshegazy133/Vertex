// Vertex

#pragma once

namespace VCore
{
template<typename FirstType, typename SecondType>
struct VPair
{
public:
	VPair() = default;
	VPair(const FirstType& InFirst, const SecondType& InSecond);
	VPair(FirstType&& InFirst, SecondType&& InSecond);
	
	template <typename TFirst, typename TSecond>
	VPair(TFirst&& InFirst, TSecond&& InSecond);
	
	bool operator==(const VPair& OtherPair) const;
	bool operator!=(const VPair& OtherPair) const;
	
public:
	FirstType FirstValue;
	SecondType SecondValue;
};
}

#include "Core/Containers/Pair.inl"