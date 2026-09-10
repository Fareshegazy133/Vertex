// Vertex

#pragma once

#include <cstddef>

namespace VCore
{
template <typename T>
class VReferencePtr
{
public:
	constexpr VReferencePtr() noexcept = default;
	constexpr VReferencePtr(const VReferencePtr&) noexcept = default;
	constexpr VReferencePtr(VReferencePtr&&) noexcept = default;
	VReferencePtr(T* InRawPtr) noexcept;
	
	void Reset(T* NewRawPtr = nullptr);
	void Swap(VReferencePtr& OtherReferencePtr) noexcept;
	
	constexpr T* Get() const;
	constexpr T& GetRef() const;
	
	bool IsValid() const;

	constexpr T* operator->() const;
	constexpr T& operator*() const;
	
	VReferencePtr& operator=(const VReferencePtr&) noexcept = default;
	VReferencePtr& operator=(VReferencePtr&&) noexcept = default;
	VReferencePtr& operator=(T* NewRawPtr) noexcept;
	VReferencePtr& operator=(std::nullptr_t) noexcept;
	
	constexpr bool operator==(const VReferencePtr& OtherReferencePtr) const noexcept;
	constexpr bool operator==(T* OtherRawPtr) const noexcept;
	constexpr bool operator!=(const VReferencePtr& OtherReferencePtr) const noexcept;
	constexpr bool operator!=(T* OtherRawPtr) const noexcept;
	
	operator T* () const;
	operator bool() const;

private:
	T* RawPtr = nullptr;
};

template <typename T>
constexpr bool operator==(std::nullptr_t, const VReferencePtr<T>& ReferencePtr) noexcept
{
	return ReferencePtr.Get() == nullptr;
}

template <typename T>
constexpr bool operator==(T* RawPtr, const VReferencePtr<T>& ReferencePtr) noexcept
{
	return RawPtr == ReferencePtr.Get();
}

template <typename T>
constexpr bool operator!=(std::nullptr_t, const VReferencePtr<T>& ReferencePtr) noexcept
{
	return ReferencePtr.Get() != nullptr;
}

template <typename T>
constexpr bool operator!=(T* RawPtr, const VReferencePtr<T>& ReferencePtr) noexcept
{
	return RawPtr != ReferencePtr.Get();
}
}

#include "Core/Memory/ReferencePtr.inl"