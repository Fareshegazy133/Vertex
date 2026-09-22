// Vertex

#pragma once

#include "Core/Reflection/Property.h"

namespace VCore
{
class VClass;

class VObjectProperty : public VProperty
{
public:
	VObjectProperty();
	VObjectProperty(VClass* InPropertyClass = nullptr);

	virtual void SetValue(void* Container, const void* Value) const override;

	virtual void GetValue(const void* Container, void* OutValue) const override;
	virtual void* GetValuePtr(void* Container) const override;
	virtual const void* GetValuePtr(const void* Container) const override;

	virtual EVPropertyType GetPropertyType() const override;

	virtual TSize GetSize() const override;
	virtual TSize GetAlignment() const override;

	VClass* GetPropertyClass() const;

private:
	VClass* PropertyClass;
};
}