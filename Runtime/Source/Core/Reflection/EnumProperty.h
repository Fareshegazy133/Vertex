// Vertex

#pragma once

#include "Core/Reflection/Property.h"

namespace VCore
{
class VEnum;

class VEnumProperty : VProperty
{
public:
	VEnumProperty();
	explicit VEnumProperty(VEnum* InEnum = nullptr);

	virtual void SetValue(void* Container, const void* Value) const override;

	virtual void GetValue(const void* Container, void* OutValue) const override;
	virtual void* GetValuePtr(void* Container) const override;
	virtual const void* GetValuePtr(const void* Container) const override;

	virtual EVPropertyType GetPropertyType() const override;

	virtual TSize GetSize() const override;
	virtual TSize GetAlignment() const override;

	VEnum* GetEnum() const;

private:
	VEnum* Enum;
};
}