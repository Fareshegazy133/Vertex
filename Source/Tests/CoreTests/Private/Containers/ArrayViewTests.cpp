// Copyright HNDRED GAMES. All Rights Reserved.

#include "Core/Containers/ArrayView.h"
#include "Core/Types/NumericTypes.h"
#include "Debug/AssertRecorder.h"
#include "doctest/doctest.h"
#include <array>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <vector>

namespace
{
	struct VBase
	{
		int32 Id = 0;
	};

	struct VDerived : VBase
	{
		int32 Extra = 0;
	};

	struct VHugeRange
	{
		int32* data() const
		{
			return nullptr;
		}

		usize size() const
		{
			return static_cast<usize>(Vertex::MaxInt32) + 1;
		}
	};

	int32 Sum(const TConstArrayView<int32> Values)
	{
		int32 Total = 0;

		for (const int32 Value : Values)
		{
			Total += Value;
		}

		return Total;
	}

	std::vector<int32> MakeScores()
	{
		return {5, 6, 7};
	}
}

static_assert(sizeof(TArrayView<int32>) == 16);
static_assert(std::is_trivially_copyable_v<TArrayView<int32>>);
static_assert(std::is_convertible_v<TArrayView<int32>, TConstArrayView<int32>>);
static_assert(!std::is_convertible_v<TConstArrayView<int32>, TArrayView<int32>>);
static_assert(std::is_constructible_v<TConstArrayView<int32>, int32*, int32>);
static_assert(!std::is_constructible_v<TArrayView<int32>, const int32*, int32>);
static_assert(!std::is_constructible_v<TArrayView<VBase>, VDerived*, int32>);
static_assert(!std::is_constructible_v<TArrayView<VBase>, TArrayView<VDerived>>);
static_assert(!std::is_constructible_v<TArrayView<VBase>, VDerived(&)[2]>);
static_assert(!std::is_constructible_v<TArrayView<int32>, const int32(&)[2]>);
static_assert(std::is_same_v<decltype(Vertex::MakeArrayView(std::declval<int32*>(), 3)), TArrayView<int32>>);
static_assert(std::is_same_v<decltype(Vertex::MakeArrayView(std::declval<const int32(&)[3]>())), TConstArrayView<int32>>);
static_assert(!std::is_constructible_v<TArrayView<VBase>, std::vector<VDerived>&>);
static_assert(!std::is_constructible_v<TArrayView<int32>, const std::vector<int32>&>);
static_assert(!std::is_constructible_v<TArrayView<bool>, std::vector<bool>&>);
static_assert(!std::is_constructible_v<TConstArrayView<int32>, std::initializer_list<int32>>);
static_assert(std::is_same_v<decltype(Vertex::MakeArrayView(std::declval<std::vector<int32>&>())), TArrayView<int32>>);
static_assert(std::is_same_v<decltype(Vertex::MakeArrayView(std::declval<const std::vector<int32>&>())), TConstArrayView<int32>>);

TEST_CASE("TArrayView: is empty when default-constructed")
{
	const TArrayView<int32> ArrayView;
	CHECK(ArrayView.Num() == 0);
	CHECK(ArrayView.IsEmpty());
	CHECK(ArrayView.GetData() == nullptr);
}

TEST_CASE("TArrayView: views a pointer and a count")
{
	int32 Scores[] = {5, 6, 7};
	const TArrayView<int32> ArrayView(Scores, 3);
	CHECK(ArrayView.Num() == 3);
	CHECK(!ArrayView.IsEmpty());
	CHECK(ArrayView.GetData() == Scores);
}

#if VERTEX_ENABLE_ASSERTS
TEST_CASE("TArrayView: asserts on a negative count, or on a null pointer with elements")
{
	int32 Scores[] = {5, 6, 7};
	CHECK_THROWS_AS(TArrayView<int32>(Scores, -1), VRecordedAssert);
	CHECK_THROWS_AS(TArrayView<int32>(static_cast<int32*>(nullptr), 2), VRecordedAssert);
}
#endif

TEST_CASE("TArrayView: [] reads and writes the viewed elements")
{
	int32 Scores[] = {5, 6, 7};
	const TArrayView<int32> ArrayView(Scores, 3);
	CHECK(ArrayView[0] == 5);
	CHECK(ArrayView[2] == 7);
	ArrayView[1] = 60;
	CHECK(Scores[1] == 60);
}

#if VERTEX_ENABLE_ASSERTS
TEST_CASE("TArrayView: [] asserts on an index outside the view")
{
	int32 Scores[] = {5, 6, 7};
	const TArrayView<int32> ArrayView(Scores, 3);
	CHECK_THROWS_AS(ArrayView[3], VRecordedAssert);
	CHECK_THROWS_AS(ArrayView[-1], VRecordedAssert);
	CHECK_THROWS_AS(TArrayView<int32>()[0], VRecordedAssert);
}
#endif

TEST_CASE("TArrayView: IsValidIndex accepts 0 to Num - 1 only")
{
	int32 Scores[] = {5, 6, 7};
	const TArrayView<int32> ArrayView(Scores, 3);
	CHECK(ArrayView.IsValidIndex(0));
	CHECK(ArrayView.IsValidIndex(2));
	CHECK(!ArrayView.IsValidIndex(3));
	CHECK(!ArrayView.IsValidIndex(-1));
	CHECK(!TArrayView<int32>().IsValidIndex(0));
}

TEST_CASE("TArrayView: range-for visits every element in order")
{
	int32 Scores[] = {5, 6, 7};
	const TArrayView<int32> ArrayView(Scores, 3);
	int32 Visits = 0;

	for (const int32 Score : ArrayView)
	{
		CHECK(Score == Scores[Visits]);
		Visits++;
	}

	CHECK(Visits == 3);
	CHECK(ArrayView.Begin() == Scores);
	CHECK(ArrayView.End() == Scores + 3);

	const TArrayView<int32> Empty;
	CHECK(Empty.Begin() == Empty.End());
}

TEST_CASE("TArrayView: converts to a read-only view of the same elements")
{
	int32 Scores[] = {5, 6, 7};
	const TArrayView<int32> ArrayView(Scores, 3);
	const TConstArrayView<int32> ReadOnly = ArrayView;
	CHECK(ReadOnly.GetData() == Scores);
	CHECK(ReadOnly.Num() == 3);
	CHECK(ReadOnly[2] == 7);
}

TEST_CASE("TArrayView: views a C array and counts its elements")
{
	int32 Scores[] = {5, 6, 7, 8};
	const TArrayView<int32> ArrayView = Scores;
	CHECK(ArrayView.Num() == 4);
	CHECK(ArrayView.GetData() == Scores);

	constexpr int32 Fixed[] = {1, 2};
	const TConstArrayView<int32> ReadOnly = Fixed;
	CHECK(ReadOnly.Num() == 2);
}

TEST_CASE("TArrayView: MakeArrayView views a pointer and a count, or a C array")
{
	int32 Scores[] = {5, 6, 7, 8};
	const auto FromPointer = Vertex::MakeArrayView(&Scores[1], 2);
	CHECK(FromPointer.GetData() == &Scores[1]);
	CHECK(FromPointer.Num() == 2);

	const auto FromArray = Vertex::MakeArrayView(Scores);
	CHECK(FromArray.GetData() == Scores);
	CHECK(FromArray.Num() == 4);
}

TEST_CASE("TArrayView: views a std::vector and a std::array")
{
	std::vector<int32> Scores = {5, 6, 7};
	const TArrayView<int32> FromVector = Scores;
	CHECK(FromVector.GetData() == Scores.data());
	CHECK(FromVector.Num() == 3);

	constexpr std::array<int32, 2> Fixed = {1, 2};
	const TConstArrayView<int32> FromArray = Fixed;
	CHECK(FromArray.GetData() == Fixed.data());
	CHECK(FromArray.Num() == 2);

	const auto Made = Vertex::MakeArrayView(Scores);
	CHECK(Made.GetData() == Scores.data());
	CHECK(Made.Num() == 3);
}

TEST_CASE("TArrayView: takes a temporary container as an argument")
{
	CHECK(Sum(MakeScores()) == 18);
}

#if VERTEX_ENABLE_ASSERTS
TEST_CASE("TArrayView: asserts when a container is too big for an int32")
{
	CHECK_THROWS_AS(TArrayView<int32>(VHugeRange{}), VRecordedAssert);
}
#endif

TEST_CASE("TArrayView: Slice views part of the elements")
{
	int32 Scores[] = {5, 6, 7, 8};
	const TArrayView<int32> ArrayView = Scores;
	const TArrayView<int32> Middle = ArrayView.Slice(1, 2);
	CHECK(Middle.GetData() == &Scores[1]);
	CHECK(Middle.Num() == 2);
	CHECK(Middle[1] == 7);
	CHECK(ArrayView.Slice(4, 0).IsEmpty());
}

#if VERTEX_ENABLE_ASSERTS
TEST_CASE("TArrayView: Slice asserts on a range outside the view")
{
	int32 Scores[] = {5, 6, 7, 8};
	const TArrayView<int32> ArrayView = Scores;
	CHECK_THROWS_AS(static_cast<void>(ArrayView.Slice(3, 2)), VRecordedAssert);
	CHECK_THROWS_AS(static_cast<void>(ArrayView.Slice(-1, 1)), VRecordedAssert);
	CHECK_THROWS_AS(static_cast<void>(ArrayView.Slice(0, -1)), VRecordedAssert);
	CHECK_THROWS_AS(static_cast<void>(ArrayView.Slice(5, 0)), VRecordedAssert);
	CHECK_THROWS_AS(static_cast<void>(ArrayView.Slice(1, Vertex::MaxInt32)), VRecordedAssert);
}
#endif

TEST_CASE("TArrayView: Find returns the first matching index, or IndexNone")
{
	int32 Scores[] = {5, 7, 6, 7};
	const TConstArrayView<int32> ArrayView = Scores;
	CHECK(ArrayView.Find(7) == 1);
	CHECK(ArrayView.Find(5) == 0);
	CHECK(ArrayView.Find(9) == Vertex::IndexNone);
	CHECK(TConstArrayView<int32>().Find(5) == Vertex::IndexNone);
}

TEST_CASE("TArrayView: Contains reports whether a value is in the view")
{
	int32 Scores[] = {5, 6, 7};
	const TConstArrayView<int32> ArrayView = Scores;
	CHECK(ArrayView.Contains(6));
	CHECK(!ArrayView.Contains(9));
	CHECK(!TConstArrayView<int32>().Contains(5));
}
