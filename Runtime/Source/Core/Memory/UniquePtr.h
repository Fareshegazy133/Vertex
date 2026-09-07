// Vertex

#pragma once

#include <type_traits>
#include <utility>

namespace VCore
{
template <typename ElementType>
class VUniquePtr
{
public:
	VUniquePtr() noexcept = default;
	VUniquePtr(ElementType* RawPtr) noexcept;
	VUniquePtr(const VUniquePtr&) = delete;
	VUniquePtr(VUniquePtr&& OtherUniquePtr) noexcept;
	
	template <typename OtherElementType> requires std::is_convertible_v<OtherElementType*, ElementType*>
	VUniquePtr(VUniquePtr<OtherElementType>&& OtherUniquePtr) noexcept;
	
	~VUniquePtr();
	
	void Reset(ElementType* NewRawPtr = nullptr);
	void Swap(VUniquePtr& OtherUniquePtr);
	
	ElementType* Release();

	ElementType* Get() const;
	bool IsValid() const;
	
	VUniquePtr& operator=(const VUniquePtr&) = delete;
	VUniquePtr& operator=(VUniquePtr&& OtherUniquePtr) noexcept;

	ElementType& operator*() const;
	ElementType* operator->() const;

	explicit operator bool() const;

private:
	template <typename>
	friend class VUniquePtr;
	
	ElementType* RawPtr = nullptr;
};
}

#include "Core/Memory/UniquePtr.inl"