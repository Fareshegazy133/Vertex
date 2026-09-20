// Vertex

#pragma once

#include <utility>

namespace VCore
{
template <typename FirstType, typename SecondType>
VPair<FirstType, SecondType>::VPair(const FirstType& InFirst, const SecondType& InSecond)
	: FirstValue(InFirst), SecondValue(InSecond)
{}

template <typename FirstType, typename SecondType>
VPair<FirstType, SecondType>::VPair(FirstType&& InFirst, SecondType&& InSecond)
	: FirstValue(std::move(InFirst)), SecondValue(std::move(InSecond))
{}

template <typename FirstType, typename SecondType>
template <typename TFirst, typename TSecond>
VPair<FirstType, SecondType>::VPair(TFirst&& InFirst, TSecond&& InSecond)
	: FirstValue(std::forward<TFirst>(InFirst)), SecondValue(std::forward<TSecond>(InSecond))
{}

template <typename FirstType, typename SecondType>
bool VPair<FirstType, SecondType>::operator==(const VPair& OtherPair) const
{
	return FirstValue == OtherPair.FirstValue && SecondValue == OtherPair.SecondValue;
}

template <typename FirstType, typename SecondType>
bool VPair<FirstType, SecondType>::operator!=(const VPair& OtherPair) const
{
	return !(*this == OtherPair);
}
}