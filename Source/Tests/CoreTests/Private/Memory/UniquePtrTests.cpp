// Copyright HNDRED GAMES. All Rights Reserved.

#include "Core/Memory/UniquePtr.h"
#include "Core/Types/NumericTypes.h"
#include "Debug/AssertRecorder.h"
#include "doctest/doctest.h"
#include <cstddef>
#include <type_traits>
#include <utility>

namespace
{
	constinit int32 Destructions = 0;

	struct VTracked
	{
		~VTracked()
		{
			Destructions++;
		}

		int32 Width = 0;
		int32 Height = 0;
	};

	constinit bool bOwnerLetGo = false;

	struct VWatcher
	{
		~VWatcher()
		{
			if (WatchedOwner)
			{
				bOwnerLetGo = WatchedOwner->Get() != this;
			}
		}

		const TUniquePtr<VWatcher>* WatchedOwner = nullptr;
	};

	struct VBase
	{
		virtual ~VBase() = default;
	};

	struct VDerived : VBase
	{
		virtual ~VDerived() override
		{
			Destructions++;
		}
	};

	struct VPlainBase
	{};

	struct VPlainDerived : VPlainBase
	{};
}

static_assert(sizeof(TUniquePtr<VTracked>) == sizeof(VTracked*));
static_assert(!std::is_convertible_v<VTracked*, TUniquePtr<VTracked>>);
static_assert(std::is_convertible_v<std::nullptr_t, TUniquePtr<VTracked>>);
static_assert(!std::is_convertible_v<TUniquePtr<VTracked>, bool>);
static_assert(!std::is_copy_constructible_v<TUniquePtr<VTracked>>);
static_assert(!std::is_copy_assignable_v<TUniquePtr<VTracked>>);
static_assert(std::is_nothrow_move_constructible_v<TUniquePtr<VTracked>>);
static_assert(std::is_nothrow_move_assignable_v<TUniquePtr<VTracked>>);
static_assert(std::is_constructible_v<TUniquePtr<VBase>, TUniquePtr<VDerived>&&>);
static_assert(std::is_constructible_v<TUniquePtr<VBase>, VDerived*>);
static_assert(std::is_constructible_v<TUniquePtr<const VTracked>, TUniquePtr<VTracked>&&>);
static_assert(std::is_constructible_v<TUniquePtr<const VTracked>, VTracked*>);
static_assert(!std::is_constructible_v<TUniquePtr<VPlainBase>, TUniquePtr<VPlainDerived>&&>);
static_assert(!std::is_constructible_v<TUniquePtr<VPlainBase>, VPlainDerived*>);
static_assert(!std::is_constructible_v<TUniquePtr<VDerived>, TUniquePtr<VBase>&&>);
static_assert(!std::is_constructible_v<TUniquePtr<VDerived>, VBase*>);
static_assert(!std::is_constructible_v<TUniquePtr<VTracked>, TUniquePtr<const VTracked>&&>);
static_assert(!std::is_constructible_v<TUniquePtr<VTracked>, const VTracked*>);

TEST_CASE("TUniquePtr: is empty when default-constructed or made from nullptr")
{
	const TUniquePtr<VTracked> Default;
	const TUniquePtr<VTracked> FromNull = nullptr;
	CHECK(Default.Get() == nullptr);
	CHECK(!Default.IsValid());
	CHECK(!Default);
	CHECK(FromNull.Get() == nullptr);
	CHECK(!FromNull.IsValid());
	CHECK(!FromNull);
}

TEST_CASE("TUniquePtr: MakeUnique builds the object from its arguments")
{
	const TUniquePtr<VTracked> Owner = Vertex::MakeUnique<VTracked>(800, 600);
	REQUIRE(Owner.IsValid());
	CHECK(Owner.Get()->Width == 800);
	CHECK(Owner.Get()->Height == 600);
}

TEST_CASE("TUniquePtr: deletes its object when it's destroyed")
{
	Destructions = 0;
	{
		const TUniquePtr<VTracked> Owner = Vertex::MakeUnique<VTracked>();
		CHECK(Destructions == 0);
	}

	CHECK(Destructions == 1);
}

#if VERTEX_ENABLE_ASSERTS
TEST_CASE("TUniquePtr: * and -> assert when it's empty")
{
	const TUniquePtr<VTracked> Empty;
	CHECK_THROWS_AS(*Empty, VRecordedAssert);
	CHECK_THROWS_AS(Empty->Width, VRecordedAssert);
}
#endif

TEST_CASE("TUniquePtr: Release hands over the object without deleting it")
{
	Destructions = 0;
	TUniquePtr<VTracked> Owner = Vertex::MakeUnique<VTracked>(800, 600);
	VTracked* const Released = Owner.Release();

	CHECK(!Owner.IsValid());
	CHECK(Destructions == 0);
	REQUIRE(Released != nullptr);
	CHECK(Released->Width == 800);

	const TUniquePtr<VTracked> NewOwner(Released);
}

TEST_CASE("TUniquePtr: Reset lets go of the object, then deletes it")
{
	TUniquePtr<VWatcher> Owner = Vertex::MakeUnique<VWatcher>();
	Owner->WatchedOwner = &Owner;
	bOwnerLetGo = false;
	Owner.Reset();
	CHECK(bOwnerLetGo);
	CHECK(!Owner.IsValid());
}

TEST_CASE("TUniquePtr: a move leaves the source empty and the object alive")
{
	Destructions = 0;
	TUniquePtr<VTracked> Source = Vertex::MakeUnique<VTracked>(800, 600);
	VTracked* const Object = Source.Get();
	const TUniquePtr<VTracked> Target = std::move(Source);

	CHECK(!Source.IsValid());
	CHECK(Target.Get() == Object);
	CHECK(Destructions == 0);
}

TEST_CASE("TUniquePtr: move assignment takes the new object, then deletes the old one")
{
	TUniquePtr<VWatcher> Owner = Vertex::MakeUnique<VWatcher>();
	Owner->WatchedOwner = &Owner;

	TUniquePtr<VWatcher> Replacement = Vertex::MakeUnique<VWatcher>();
	VWatcher* const NewObject = Replacement.Get();
	bOwnerLetGo = false;
	Owner = std::move(Replacement);

	CHECK(bOwnerLetGo);
	CHECK(Owner.Get() == NewObject);
	CHECK(!Replacement.IsValid());
}

TEST_CASE("TUniquePtr: move-assigning to itself keeps the object")
{
	Destructions = 0;
	TUniquePtr<VTracked> Owner = Vertex::MakeUnique<VTracked>();
	VTracked* const Object = Owner.Get();
	Owner = std::move(Owner);

	CHECK(Owner.Get() == Object);
	CHECK(Destructions == 0);
}

TEST_CASE("TUniquePtr: deletes a derived object through its base")
{
	Destructions = 0;
	{
		const TUniquePtr<VBase> Owner = Vertex::MakeUnique<VDerived>();
		CHECK(Destructions == 0);
	}

	CHECK(Destructions == 1);
}

TEST_CASE("TUniquePtr: Release and Reset are safe on an empty owner")
{
	TUniquePtr<VTracked> Owner;
	CHECK(Owner.Release() == nullptr);
	Owner.Reset();
	CHECK(!Owner.IsValid());
}
