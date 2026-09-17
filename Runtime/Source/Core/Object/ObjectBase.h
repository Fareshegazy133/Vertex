// Vertex

#pragma once

namespace VCore
{
class VClass;

class VObjectBase
{
public:
	virtual ~VObjectBase() = default;

	virtual VClass* GetClass() const = 0;

	template<typename T>
	bool IsA() const;
	
	bool IsA(const VClass* Class) const;
};

template <typename T>
bool VObjectBase::IsA() const
{
	return IsA(T::StaticClass());
}
}