// Copyright HNDRED GAMES. All Rights Reserved.

#pragma once

#include "Core/Debug/Assert.h"
#include "Core/Types/NumericTypes.h"
#include <type_traits>
#include <utility>

/**
 * A view of elements that sit next to each other in memory: a pointer to the first one and a count. A function that only reads or edits a list takes one, so its caller can pass whatever holds the elements.
 *
 * @tparam TElement The viewed elements' type. Make it const for a read-only view, written TConstArrayView<T>: a const TArrayView<T> can still write its elements, as an int32* const can.
 * @ownership Owns nothing. The elements belong to whatever the view was made from. Copying a view copies the pointer and the count, never the elements.
 * @lifetime Valid only while its elements stay where they are. It dangles as soon as its source is destroyed, or grows and moves its elements, so never keep one longer than its source.
 * @threading Not synchronized, like a raw pointer. The elements' thread safety is up to their owner.
 * @networking Local only. A pointer means nothing on another machine.
 * @performance 16 bytes, trivially copyable, and never allocates: pass it by value. Num, IsEmpty, GetData, and iteration are plain reads. [] and Slice add one bounds check in Debug and Development, and none in Shipping. Find and Contains are linear searches.
 * @invariants Count is never negative, and Data is only null when Count is 0.
 */
template <typename TElement>
class TArrayView
{
public:
	/** Makes an empty view: no elements, and a null pointer. */
	TArrayView() = default;

	/**
	 * Views InCount elements, starting at InData.
	 *
	 * @tparam TOtherElement The pointer's real element type, deduced from InData. Deducing it, instead of taking a TElement*, keeps a derived type visible to the constraint.
	 * @param InData The first element. May be nullptr only when InCount is 0, and then it needs a type: static_cast<int32*>(nullptr), not a bare nullptr.
	 * @param InCount How many elements to view. Never negative.
	 * @warning Asserts when InCount is negative, or when InData is null and InCount isn't 0.
	 * @warning Nothing checks that InCount elements really live at InData. A count that's too large lets the view read past them.
	 * @note Compiles only when TOtherElement is TElement, give or take const. A view of a base type over derived elements would step through memory at the base's size and read every element after the first from the wrong address.
	 */
	template <typename TOtherElement> requires std::is_convertible_v<TOtherElement(*)[], TElement(*)[]>
	TArrayView(TOtherElement* const InData, const int32 InCount);

	/**
	 * Views every element of a C array. Implicit, so a function that takes a view accepts the array itself.
	 *
	 * @tparam ArrayNum The array's length, deduced from its type. An array longer than an int32 can count doesn't match.
	 * @param Array The array to view.
	 * @note Takes the array by reference, so it never decays to a pointer and loses its length. A reference to an array only binds when the element types match, give or take const, so it needs no stride guard.
	 */
	template <int32 ArrayNum>
	TArrayView(TElement(&Array)[ArrayNum]);

	/**
	 * Views every element of a container Vertex doesn't own, such as a std::vector or a std::array: any type with data() and size(). Implicit, so a function that takes a view accepts the container itself.
	 *
	 * @tparam TRange The container's type, deduced from Range. A const container deduces as const, so it only fits a TConstArrayView.
	 * @param Range The container to view. A temporary is accepted.
	 * @warning Asserts when Range holds more elements than an int32 can count.
	 * @warning The view dangles as soon as Range adds or removes an element, reallocates, or is destroyed. A view of a temporary is only safe as an argument: Sum(MakeScores()) is fine, but a stored view of it is not.
	 * @note Compiles only when data() points at TElement, give or take const. That rejects std::vector<bool> and std::initializer_list, which have no data(), and a container of derived elements for a view of their base.
	 * @note Vertex's own containers convert to a view in their own headers, so this header never names them.
	 */
	template <typename TRange> requires requires (TRange& Range) { Range.data(); Range.size(); } && std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<TRange&>().data())>(*)[], TElement(*)[]>
	TArrayView(TRange&& Range);

	/**
	 * Views the same elements as another view. Implicit, so a TArrayView<T> passes wherever a TConstArrayView<T> is expected.
	 *
	 * @tparam TOtherElement The other view's element type, deduced from OtherArrayView.
	 * @param OtherArrayView The view to take the pointer and the count from.
	 * @note Compiles only when TOtherElement is TElement, give or take const. It can add const but never remove it, and never turns derived elements into base ones.
	 */
	template <typename TOtherElement> requires std::is_convertible_v<TOtherElement(*)[], TElement(*)[]>
	TArrayView(const TArrayView<TOtherElement>& OtherArrayView);

	/**
	 * Views part of this view's elements, without copying them.
	 *
	 * @param Index The part's first element, from 0 to Num.
	 * @param SliceCount How many elements the part covers, from 0 to Num - Index.
	 * @return A view of SliceCount elements, starting at element Index. It points into the same memory, so it dangles when this view's source does.
	 * @warning Asserts when the range isn't inside the view, including when Index + SliceCount would overflow an int32. Shipping has no check, so a bad range there is undefined behavior.
	 * @note Ignoring the result fails the build (C4834): Slice returns a new view and never changes this one.
	 */
	[[nodiscard]] TArrayView Slice(const int32 Index, const int32 SliceCount) const;

	/**
	 * Reads the first element's address, where a loop over the view starts.
	 *
	 * @return The first element. Equal to End when the view is empty, and may be nullptr then.
	 */
	TElement* Begin() const;

	/** Begin, spelled the way range-for looks it up, so for (Element : View) works. */
	TElement* begin() const;

	/**
	 * Reads the address one past the last element, where a loop over the view stops.
	 *
	 * @return Begin plus Num.
	 * @warning Never dereference the result: no element lives there.
	 */
	TElement* End() const;

	/** End, spelled the way range-for looks it up. */
	TElement* end() const;

	/**
	 * Counts the viewed elements.
	 *
	 * @return 0 or more. An int32, like every count and index in Vertex's containers.
	 */
	int32 Num() const;

	/**
	 * Searches the view for the first element equal to Value.
	 *
	 * @param Value The value to look for, compared with TElement's ==.
	 * @return The first matching index, or Vertex::IndexNone when no element matches.
	 * @note A linear search: it reads up to Num elements.
	 * @note Only compiled when it's called, so a view of a type without == still works until something calls Find or Contains.
	 */
	int32 Find(const TElement& Value) const;

	/**
	 * Reads the first element's address, for code that takes a pointer and a count, such as a C API.
	 *
	 * @return The first element. May be nullptr when the view is empty: a container's GetData is an exception to the rule that a Get never comes back empty.
	 * @warning Read at most Num elements through it.
	 */
	TElement* GetData() const;

	/**
	 * Reports whether the view has no elements.
	 *
	 * @return True when Num is 0.
	 */
	bool IsEmpty() const;

	/**
	 * Reports whether an index names an element of the view.
	 *
	 * @param Index The index to test. Any value is safe, negative ones included.
	 * @return True when Index is from 0 to Num - 1.
	 */
	bool IsValidIndex(const int32 Index) const;

	/**
	 * Reports whether any element equals Value.
	 *
	 * @param Value The value to look for, compared with TElement's ==.
	 * @return True when Find would return an index.
	 * @note A linear search, like Find.
	 */
	bool Contains(const TElement& Value) const;

	/**
	 * Reaches one element: View[Index].
	 *
	 * @param Index The element's position, from 0 to Num - 1.
	 * @return The element itself. Writable unless TElement is const, even through a const view.
	 * @warning Asserts when Index is outside the view. Shipping has no check, so a bad index there is undefined behavior: test IsValidIndex first when it may be out of range.
	 * @note Never noexcept: CoreTests' assert handler throws through it, and noexcept would turn that throw into std::terminate.
	 */
	TElement& operator[](const int32 Index) const;

private:
	/** The first viewed element. Only null when Count is 0. The view never frees it. */
	TElement* Data = nullptr;

	/** How many elements the view covers. Never negative: the constructor asserts it. */
	int32 Count = 0;
};

template <typename TElement>
using TConstArrayView = TArrayView<const TElement>;

namespace Vertex
{
	/**
	 * Views Count elements starting at Data, with the element type deduced from the pointer: auto View = Vertex::MakeArrayView(Pointer, Count).
	 *
	 * @tparam TElement The element type, deduced from Data. A pointer to const gives a read-only view.
	 * @param Data The first element. May be nullptr only when Count is 0.
	 * @param Count How many elements to view. Never negative.
	 * @return A view of the Count elements.
	 * @warning Asserts like the pointer-and-count constructor: when Count is negative, or when Data is null and Count isn't 0.
	 * @note It exists because TArrayView(Pointer, Count) can't deduce the view's element type (C2641): that constructor's parameter names TOtherElement, not TElement.
	 */
	template <typename TElement>
	[[nodiscard]] TArrayView<TElement> MakeArrayView(TElement* const Data, const int32 Count)
	{
		return TArrayView<TElement>(Data, Count);
	}

	/**
	 * Views every element of a C array, with the element type deduced from the array.
	 *
	 * @tparam TElement The element type, deduced from Array. A const array gives a read-only view.
	 * @tparam ArrayNum The array's length, deduced from its type.
	 * @param Array The array to view.
	 * @return A view of all ArrayNum elements.
	 */
	template <typename TElement, int32 ArrayNum>
	[[nodiscard]] TArrayView<TElement> MakeArrayView(TElement (&Array)[ArrayNum])
	{
		return TArrayView<TElement>(Array);
	}

	/**
	 * Views every element of a container Vertex doesn't own, with the element type deduced from its data(): auto View = Vertex::MakeArrayView(Scores).
	 *
	 * @tparam TRange The container's type, deduced from Range. A const container gives a read-only view.
	 * @param Range The container to view: any type with data() and size().
	 * @return A view of all of Range's elements.
	 * @warning Asserts when Range holds more elements than an int32 can count.
	 * @warning Never keep the view of a temporary: the temporary dies at the end of the statement, and the view dangles.
	 */
	template <typename TRange> requires requires (TRange& Range) { Range.data(); Range.size(); }
	[[nodiscard]] auto MakeArrayView(TRange&& Range) -> TArrayView<std::remove_pointer_t<decltype(Range.data())>>
	{
		return TArrayView<std::remove_pointer_t<decltype(Range.data())>>(Range);
	}
}

template <typename TElement>
template <typename TOtherElement> requires std::is_convertible_v<TOtherElement(*)[], TElement(*)[]>
TArrayView<TElement>::TArrayView(TOtherElement* const InData, const int32 InCount)
	: Data(InData), Count(InCount)
{
	VX_ASSERT(Count >= 0, "A view can't hold {} elements", Count);
	VX_ASSERT(Data != nullptr || Count == 0, "A null pointer can't hold {} elements", Count);
}

template <typename TElement>
template <int32 ArrayNum>
TArrayView<TElement>::TArrayView(TElement(&Array)[ArrayNum])
	: Data(Array), Count(ArrayNum)
{}

template <typename TElement>
template <typename TRange> requires requires (TRange& Range) { Range.data(); Range.size(); } && std::is_convertible_v<std::remove_pointer_t<decltype(std::declval<TRange&>().data())>(*)[], TElement(*)[]>
TArrayView<TElement>::TArrayView(TRange&& Range)
	: Data(Range.data()), Count(static_cast<int32>(Range.size()))
{
	VX_ASSERT(Range.size() <= static_cast<usize>(Vertex::MaxInt32), "{} elements don't fit in a view", Range.size());
}

template <typename TElement>
template <typename TOtherElement> requires std::is_convertible_v<TOtherElement(*)[], TElement(*)[]>
TArrayView<TElement>::TArrayView(const TArrayView<TOtherElement>& OtherArrayView)
	: Data(OtherArrayView.GetData()), Count(OtherArrayView.Num())
{}

template <typename TElement>
TArrayView<TElement> TArrayView<TElement>::Slice(const int32 Index, const int32 SliceCount) const
{
	VX_ASSERT(Index >= 0 && SliceCount >= 0 && Index <= Count && SliceCount <= Count - Index, "Slice({}, {}) is outside a view of {}", Index, SliceCount, Count);
	return TArrayView(Data + Index, SliceCount);
}

template <typename TElement>
TElement* TArrayView<TElement>::Begin() const
{
	return Data;
}

template <typename TElement>
TElement* TArrayView<TElement>::begin() const
{
	return Data;
}

template <typename TElement>
TElement* TArrayView<TElement>::End() const
{
	return Data + Count;
}

template <typename TElement>
TElement* TArrayView<TElement>::end() const
{
	return Data + Count;
}

template <typename TElement>
int32 TArrayView<TElement>::Num() const
{
	return Count;
}

template <typename TElement>
int32 TArrayView<TElement>::Find(const TElement& Value) const
{
	for (int32 Index = 0; Index < Count; Index++)
	{
		if (Data[Index] == Value) return Index;
	}

	return Vertex::IndexNone;
}

template <typename TElement>
TElement* TArrayView<TElement>::GetData() const
{
	return Data;
}

template <typename TElement>
bool TArrayView<TElement>::IsEmpty() const
{
	return Count == 0;
}

template <typename TElement>
bool TArrayView<TElement>::IsValidIndex(const int32 Index) const
{
	return Index >= 0 && Index < Count;
}

template <typename TElement>
bool TArrayView<TElement>::Contains(const TElement& Value) const
{
	return Find(Value) != Vertex::IndexNone;
}

template <typename TElement>
TElement& TArrayView<TElement>::operator[](const int32 Index) const
{
	VX_ASSERT(IsValidIndex(Index), "Index {} is outside a view of {}", Index, Count);
	return Data[Index];
}
