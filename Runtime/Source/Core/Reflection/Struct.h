// Vertex

#pragma once

#include "Core/Reflection/Field.h"

namespace VCore
{
class VProperty;

class VStruct : public VField
{
public:
	VStruct(VStruct* InParentStruct = nullptr, const VString& InName = VString::NullString, const TSize InSize = 0);
	virtual ~VStruct() override;
	
	virtual void Initialize(void* Data) const;
	virtual void Destroy(void* Data) const;
	virtual void Copy(void* Dest, const void* Source) const;

	void AddProperty(VProperty* Property);
	
	void SetStructSize(const TSize InStructSize);
	void SetStructAlignment(const TSize InStructAlignment);
	
	VProperty* FindProperty(const VString& PropertyName) const;
	
	VStruct* GetParentStruct() const;
	VProperty* GetProperties() const;
	TSize GetStructAlignment() const;
	TSize GetStructSize() const;

protected:
	VStruct* ParentStruct;
	VProperty* Properties;

	TSize StructAlignment;
	TSize StructSize;
};
}