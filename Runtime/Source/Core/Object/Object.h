// Vertex

#pragma once

#include "Core/CoreAliases.h"
#include "Core/Object/ObjectBase.h"
#include "Core/Object/ObjectFlags.h"
#include "Core/Reflection/Property.h"
#include "Core/Types/Name.h"

namespace VCore
{
class VClass;

class VObject : public VObjectBase
{
public:
	VObject();
	virtual ~VObject() override = default;

	template<typename T>
	T& GetPropertyValue(VObject* Object, VProperty* Property);

	virtual VClass* GetClass() const override;
	static VClass* StaticClass();
	
	void AddToRoot();
	void RemoveFromRoot();
	
	void AddFlags(EVObjectFlags InObjectFlags);
	void RemoveFlags(EVObjectFlags InObjectFlags);
	
	void MarkPendingKill();
	
	void SetOuter(VObject* InOuter);
	void SetName(const VName& InName);
	
	VObject* GetOuter() const;
	VObject* GetOuterMost() const;
	EVObjectFlags GetObjectFlags() const;
	VName GetName() const;
	VString GetPathName() const;
	
	bool HasAnyObjectFlags(EVObjectFlags InObjectFlags) const;
	bool HasAllObjectFlags(EVObjectFlags InObjectFlags) const;
	bool IsRootObject() const;
	bool IsPendingKill() const;

private:
	VObject* Outer;
	EVObjectFlags ObjectFlags;
	VName Name = NAME_None;
};

template<typename T>
T& VObject::GetPropertyValue(VObject* Object, VProperty* Property)
{
	return *static_cast<T*>(Property->GetValuePtr(Object));
}
}