// Vertex

#pragma once

#include "Core/Reflection/Field.h"

namespace VCore
{
class VObject;

enum class EVPropertyType
{
	Unknown,
	Bool,
	Int8,
	UInt8,
	Int16,
	UInt16,
	Int32,
	UInt32,
	Int64,
	UInt64,
	Float32,
	Float64,
	String,
	Name,
	Object,
	Struct,
	Enum,
	Array,
	Map,
	Set
};

class VProperty : public VField
{
public:
	VProperty() = default;
	virtual ~VProperty() override = default;
	
	virtual void SetValue(void* Container, const void* Value) const = 0;
	
	virtual void GetValue(const void* Container, void* OutValue) const = 0;
	virtual void* GetValuePtr(void* Container) const = 0;
	virtual const void* GetValuePtr(const void* Container) const = 0;
	
	virtual EVPropertyType GetPropertyType() const = 0;
	
	virtual TSize GetSize() const = 0;
	virtual TSize GetAlignment() const = 0;
	
	void SetOffset(const TSize InOffset);
	void SetIsEditable(const bool IsEditable);

	TSize GetOffset() const;
	bool IsEditable() const;

protected:
	TSize Offset = 0;
	bool bIsEditable = true;
};
}