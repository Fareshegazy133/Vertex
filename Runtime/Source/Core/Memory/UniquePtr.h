// Vertex

#pragma once

#include <type_traits>

namespace VCore
{
template <typename T>
class VUniquePtr
{
public:
	VUniquePtr() noexcept = default;
	VUniquePtr(T* InRawPtr) noexcept;
	VUniquePtr(const VUniquePtr&) = delete;
	VUniquePtr(VUniquePtr&& OtherUniquePtr) noexcept;
	
	template <typename OtherT> requires std::is_convertible_v<OtherT*, T*>
	VUniquePtr(VUniquePtr<OtherT>&& OtherUniquePtr) noexcept;
	
	~VUniquePtr();
	
	void Reset(T* NewRawPtr = nullptr);
	void Swap(VUniquePtr& OtherUniquePtr);
	
	T* Release();

	T* Get() const;
	bool IsValid() const;
	
	VUniquePtr& operator=(const VUniquePtr&) = delete;
	VUniquePtr& operator=(VUniquePtr&& OtherUniquePtr) noexcept;

	T& operator*() const;
	T* operator->() const;

	operator T* () const;
	operator bool() const;

private:
	template <typename>
	friend class VUniquePtr;
	
	T* RawPtr = nullptr;
};

template <typename T, typename... Args>
VUniquePtr<T> MakeUnique(Args&&... Arguments);
}

#include "Core/Memory/UniquePtr.inl"