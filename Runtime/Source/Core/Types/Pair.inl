// Vertex

#pragma once

#include <utility>

namespace VCore
{
template <typename FirstType, typename SecondType>
VPair<FirstType, SecondType>::VPair(const FirstType& InFirst, const SecondType& InSecond)
	: First(InFirst), Second(InSecond)
{}

template <typename FirstType, typename SecondType>
VPair<FirstType, SecondType>::VPair(FirstType&& InFirst, SecondType&& InSecond)
	: First(std::move(InFirst)), Second(std::move(InSecond))
{}

template <typename FirstType, typename SecondType>
template <typename TFirst, typename TSecond>
VPair<FirstType, SecondType>::VPair(TFirst&& InFirst, TSecond&& InSecond)
	: First(std::forward<TFirst>(InFirst)), Second(std::forward<TSecond>(InSecond))
{}
}