// Copyright HNDRED GAMES. All Rights Reserved.

#pragma once

#include "Core/Debug/Assert.h"
#include <cstddef>
#include <type_traits>
#include <utility>

/**
 * Owns one heap object and deletes it exactly once, when the owner is destroyed. It's Vertex's single-ownership pointer, in place of std::unique_ptr and naked new and delete.
 *
 * @tparam TElement The owned object's type. Never an array: TArray owns arrays.
 * @ownership Owns the object it points to, and nothing else may delete it. Move-only: a move hands the object to the new owner and leaves the source empty. Copying is deleted, because two copies would delete the same object. An owner of a derived type converts to an owner of its base only when the base has a virtual destructor.
 * @lifetime Empty when default-constructed or made from nullptr. The owned object lives until the owner is destroyed or Reset deletes it, or until Release hands it to the caller.
 * @threading Not synchronized, like a raw pointer: one thread at a time may use an owner. The owned object's own thread safety is up to its type.
 * @networking Local only. A pointer means nothing on another machine.
 * @performance The size of a raw pointer. It never allocates: MakeUnique's single new is the only allocation. Get, IsValid, and operator bool are plain pointer reads. * and -> add one check in Debug and Development, and none in Shipping. A move is two pointer writes, and both moves are noexcept, so a container can move owners as it grows.
 * @invariants Holds nullptr, or a pointer that only this owner will delete.
 */
template <typename TElement> requires (!std::is_array_v<TElement>)
class TUniquePtr
{
public:
	/** Makes an empty owner. */
	TUniquePtr() = default;

	/** Makes an empty owner. Implicit, so a function that returns a TUniquePtr can return nullptr. */
	TUniquePtr(const std::nullptr_t);

	/**
	 * Takes ownership of an object that's already on the heap. Explicit, so a raw pointer never becomes an owner without the code asking.
	 *
	 * @tparam TOtherElement The object's real type, deduced from Data. Deducing it, instead of taking a TElement*, keeps a derived type visible to the constraint.
	 * @param Data The object to own, or nullptr.
	 * @warning Data must come from new, and nothing else may own or delete it afterwards. Prefer Vertex::MakeUnique, which never exposes the raw pointer.
	 * @note Compiles only when deleting Data through a TElement* is safe: TOtherElement is TElement, give or take const, or TElement has a virtual destructor.
	 */
	template <typename TOtherElement> requires std::is_convertible_v<TOtherElement*, TElement*> && (std::is_same_v<std::remove_cv_t<TOtherElement>, std::remove_cv_t<TElement>> || std::has_virtual_destructor_v<TElement>)
	explicit TUniquePtr(TOtherElement* const Data);

	/** Deleted: two owners would delete the same object. */
	TUniquePtr(const TUniquePtr&) = delete;

	/**
	 * Takes the object from another owner, which is left empty.
	 *
	 * @param OtherUniquePtr The owner to take from. Empty afterwards.
	 */
	TUniquePtr(TUniquePtr&& OtherUniquePtr) noexcept;

	/**
	 * Takes the object from an owner of a derived or less const type, which is left empty. Implicit, so a derived owner converts wherever a base owner is expected.
	 *
	 * @tparam TOtherElement The other owner's element type, deduced from OtherUniquePtr.
	 * @param OtherUniquePtr The owner to take from. Empty afterwards.
	 * @note Compiles only when deleting the object through a TElement* is safe: TOtherElement is TElement, give or take const, or TElement has a virtual destructor. Otherwise, the conversion fails with C2440.
	 */
	template <typename TOtherElement> requires std::is_convertible_v<TOtherElement*, TElement*> && (std::is_same_v<std::remove_cv_t<TOtherElement>, std::remove_cv_t<TElement>> || std::has_virtual_destructor_v<TElement>)
	TUniquePtr(TUniquePtr<TOtherElement>&& OtherUniquePtr) noexcept;

	/**
	 * Deletes the owned object, if there is one.
	 *
	 * @warning TElement must be a complete type wherever this destructor is compiled. A class that owns a TUniquePtr to a type its header only declares must declare its own destructor and move operations in that header, and define them in its .cpp, where the type is complete. Otherwise, the build fails with C2027.
	 */
	~TUniquePtr();

	/**
	 * Deletes the owned object, if there is one, and leaves the owner empty.
	 *
	 * @note The owner is already empty when the object's destructor runs. A destructor that reaches back into its owner finds it empty, not pointing at an object that's halfway destroyed.
	 */
	void Reset();

	/**
	 * Hands the owned object to the caller and leaves the owner empty. Nothing is deleted.
	 *
	 * @return The object, which the caller now owns, or nullptr when the owner was empty.
	 * @warning The caller must give the result to a new owner, as in TUniquePtr<VFoo>(Released), or to code that takes ownership of it. Ignoring the result fails the build, because the object would leak.
	 */
	[[nodiscard]] TElement* Release();

	/**
	 * Reads the owned object's address without giving up ownership.
	 *
	 * @return The owned object, or nullptr when the owner is empty. Pointer types are the one exception to the rule that a Get never comes back empty.
	 * @warning Never delete the result: the owner still will.
	 */
	TElement* Get() const;

	/**
	 * Reports whether the owner holds an object.
	 *
	 * @return True when it does; false when it's empty.
	 */
	bool IsValid() const;

	/** Deleted, like the copy constructor. */
	TUniquePtr& operator=(const TUniquePtr&) = delete;

	/**
	 * Takes the object from another owner, then deletes the object this owner held, if there was one.
	 *
	 * @param OtherUniquePtr The owner to take from. Empty afterwards, unless it's this owner: a move into itself keeps the object.
	 * @return This owner.
	 * @note The old object is deleted last, so its destructor finds this owner already holding the new one.
	 */
	TUniquePtr& operator=(TUniquePtr&& OtherUniquePtr) noexcept;

	/**
	 * Reaches a member of the owned object: Owner->Member.
	 *
	 * @return The owned object. Never nullptr.
	 * @warning Asserts when the owner is empty. Shipping has no check, so an empty owner there is undefined behavior: test IsValid first when it may be empty.
	 * @note A const owner still reaches a non-const object, as a T* const does. For a read-only object, own a TUniquePtr<const T>.
	 * @note Never noexcept: CoreTests' assert handler throws through it, and noexcept would turn that throw into std::terminate.
	 */
	TElement* operator->() const;

	/**
	 * Reaches the owned object itself: *Owner.
	 *
	 * @return The owned object.
	 * @warning Asserts when the owner is empty. Shipping has no check, so an empty owner there is undefined behavior: test IsValid first when it may be empty.
	 * @note A const owner still reaches a non-const object, as a T* const does. For a read-only object, own a TUniquePtr<const T>.
	 * @note Never noexcept: CoreTests' assert handler throws through it, and noexcept would turn that throw into std::terminate.
	 */
	TElement& operator*() const;

	/**
	 * Reports whether the owner holds an object, so an owner can be tested with if (Owner).
	 *
	 * @return The same as IsValid.
	 * @note Explicit: an owner never turns into a bool by accident, such as in arithmetic or when passed to a bool parameter.
	 */
	explicit operator bool() const;

private:
	/**
	 * Deletes one object. Every path that deletes goes through here, so the complete-type check covers them all.
	 *
	 * @param InElement The object to delete, or nullptr, which does nothing.
	 * @warning Fails the build with C2027 when TElement is only declared. Without the check, delete would compile with warning C4150, free the memory, and skip the object's destructor.
	 */
	static void DeleteElement(const TElement* const InElement);

private:
	/** The owned object, or nullptr when the owner is empty. Only this owner deletes it. */
	TElement* Element = nullptr;
};

namespace Vertex
{
	/**
	 * Creates an object on the heap and returns its owner, so the raw pointer never appears in the calling code.
	 *
	 * @tparam TElement The type to create. The caller spells it out: MakeUnique<VFoo>(…).
	 * @tparam TArguments The types of the values to build it from. Deduced from Arguments; never spelled out.
	 * @param Arguments Forwarded to TElement's constructor, or to its members in order when TElement is an aggregate.
	 * @return The owner of the new object. Never empty.
	 * @note Allocates once, with new.
	 */
	template <typename TElement, typename... TArguments>
	[[nodiscard]] TUniquePtr<TElement> MakeUnique(TArguments&&... Arguments)
	{
		return TUniquePtr<TElement>(new TElement(std::forward<TArguments>(Arguments)...));
	}
}

template <typename TElement> requires (!std::is_array_v<TElement>)
TUniquePtr<TElement>::TUniquePtr(const std::nullptr_t)
{}

template <typename TElement> requires (!std::is_array_v<TElement>)
template <typename TOtherElement> requires std::is_convertible_v<TOtherElement*, TElement*> && (std::is_same_v<std::remove_cv_t<TOtherElement>, std::remove_cv_t<TElement>> || std::has_virtual_destructor_v<TElement>)
TUniquePtr<TElement>::TUniquePtr(TOtherElement* const Data)
	: Element(Data)
{}

template <typename TElement> requires (!std::is_array_v<TElement>)
TUniquePtr<TElement>::TUniquePtr(TUniquePtr&& OtherUniquePtr) noexcept
	: Element(OtherUniquePtr.Release())
{}

template <typename TElement> requires (!std::is_array_v<TElement>)
template <typename TOtherElement> requires std::is_convertible_v<TOtherElement*, TElement*> && (std::is_same_v<std::remove_cv_t<TOtherElement>, std::remove_cv_t<TElement>> || std::has_virtual_destructor_v<TElement>)
TUniquePtr<TElement>::TUniquePtr(TUniquePtr<TOtherElement>&& OtherUniquePtr) noexcept
	: Element(OtherUniquePtr.Release())
{}

template <typename TElement> requires (!std::is_array_v<TElement>)
TUniquePtr<TElement>::~TUniquePtr()
{
	DeleteElement(Element);
}

template <typename TElement> requires (!std::is_array_v<TElement>)
void TUniquePtr<TElement>::Reset()
{
	DeleteElement(Release());
}

template <typename TElement> requires (!std::is_array_v<TElement>)
TElement* TUniquePtr<TElement>::Release()
{
	TElement* const ReleasedElement = Element;
	Element = nullptr;
	return ReleasedElement;
}

template <typename TElement> requires (!std::is_array_v<TElement>)
TElement* TUniquePtr<TElement>::Get() const
{
	return Element;
}

template <typename TElement> requires (!std::is_array_v<TElement>)
bool TUniquePtr<TElement>::IsValid() const
{
	return Element;
}

template <typename TElement> requires (!std::is_array_v<TElement>)
TUniquePtr<TElement>& TUniquePtr<TElement>::operator=(TUniquePtr&& OtherUniquePtr) noexcept
{
	if (this != &OtherUniquePtr)
	{
		const TElement* const OldElement = Element;
		Element = OtherUniquePtr.Release();
		DeleteElement(OldElement);
	}

	return *this;
}

template <typename TElement> requires (!std::is_array_v<TElement>)
TElement* TUniquePtr<TElement>::operator->() const
{
	VX_ASSERT(IsValid(), "Dereferenced an empty TUniquePtr");
	return Element;
}

template <typename TElement> requires (!std::is_array_v<TElement>)
TElement& TUniquePtr<TElement>::operator*() const
{
	VX_ASSERT(IsValid(), "Dereferenced an empty TUniquePtr");
	return *Element;
}

template <typename TElement> requires (!std::is_array_v<TElement>)
TUniquePtr<TElement>::operator bool() const
{
	return IsValid();
}

template <typename TElement> requires (!std::is_array_v<TElement>)
void TUniquePtr<TElement>::DeleteElement(const TElement* const InElement)
{
	static_assert(sizeof(TElement) > 0);
	delete InElement;
}
