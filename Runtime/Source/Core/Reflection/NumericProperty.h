// Vertex

#pragma once

#include "Core/Reflection/Property.h"

namespace VCore
{
template<typename T, EVPropertyType PropertyType>
class VNumericProperty : public VProperty
{
public:
	virtual void SetValue(void* Container, const void* Value) const override;

	virtual void GetValue(const VObject* Object, void* OutValue) const override;
	virtual void* GetValuePtr(void* Container) const override;
	virtual const void* GetValuePtr(const void* Container) const override;
	
	virtual EVPropertyType GetPropertyType() const override;
	
	virtual TSize GetSize() const override;
	virtual TSize GetAlignment() const override;
};

template <typename T, EVPropertyType PropertyType>
void VNumericProperty<T, PropertyType>::SetValue(void* Container, const void* Value) const
{
	*reinterpret_cast<T*>(static_cast<char*>(Container) + Offset) = *static_cast<const T*>(Value);
}

template <typename T, EVPropertyType PropertyType>
void VNumericProperty<T, PropertyType>::GetValue(const VObject* Object, void* OutValue) const
{
	*static_cast<T*>(OutValue) = *reinterpret_cast<const T*>(reinterpret_cast<const char*>(Object) + Offset);
}

template <typename T, EVPropertyType PropertyType>
void* VNumericProperty<T, PropertyType>::GetValuePtr(void* Container) const
{
	return static_cast<char*>(Container) + Offset;
}

template <typename T, EVPropertyType PropertyType>
const void* VNumericProperty<T, PropertyType>::GetValuePtr(const void* Container) const
{
	return static_cast<const char*>(Container) + Offset;
}

template <typename T, EVPropertyType PropertyType>
EVPropertyType VNumericProperty<T, PropertyType>::GetPropertyType() const
{
	return PropertyType;
}

template <typename T, EVPropertyType PropertyType>
TSize VNumericProperty<T, PropertyType>::GetSize() const
{
	return sizeof(T);
}

template <typename T, EVPropertyType PropertyType>
TSize VNumericProperty<T, PropertyType>::GetAlignment() const
{
	return alignof(T);
}

using VBoolProperty = VNumericProperty<bool, EVPropertyType::Bool>;
using VInt8Property = VNumericProperty<int8, EVPropertyType::Int8>;
using VUInt8Property = VNumericProperty<uint8, EVPropertyType::UInt8>;
using VInt16Property = VNumericProperty<int16, EVPropertyType::Int16>;
using VUInt16Property = VNumericProperty<uint16, EVPropertyType::UInt16>;
using VInt32Property = VNumericProperty<int32, EVPropertyType::Int32>;
using VUInt32Property = VNumericProperty<uint32, EVPropertyType::UInt32>;
using VInt64Property = VNumericProperty<int64, EVPropertyType::Int64>;
using VUInt64Property = VNumericProperty<uint64, EVPropertyType::UInt64>;
using VFloat32Property = VNumericProperty<float32, EVPropertyType::Float32>;
using VFloat64Property = VNumericProperty<float64, EVPropertyType::Float64>;
}