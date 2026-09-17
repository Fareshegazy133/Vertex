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
	Object,
	Struct,
	Array
};

class VProperty : public VField
{
public:
	VProperty() = default;
	virtual ~VProperty() override = default;
	
	virtual void SetValue(VObject* Object, const void* Value) const = 0;
	
	void SetOffset(const TSize InOffset);
	void SetIsEditable(const bool IsEditable);
	
	virtual void GetValue(const VObject* Object, void* OutValue) const = 0;
	virtual void* GetValuePtr(VObject* Object) const = 0;
	virtual const void* GetValuePtr(const VObject* Object) const = 0;
	
	virtual EVPropertyType GetPropertyType() const = 0;
	
	virtual TSize GetSize() const = 0;
	virtual TSize GetAlignment() const = 0;

	TSize GetOffset() const;
	bool IsEditable() const;

protected:
	TSize Offset = 0;
	bool bIsEditable = true;
};
}