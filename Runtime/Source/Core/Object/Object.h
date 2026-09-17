// Vertex

#pragma once

#include "Core/CoreAliases.h"
#include "Core/Object/ObjectBase.h"
#include "Core/Object/ObjectFlags.h"

namespace VCore
{
class VClass;
class VObject;

class VObject : public VObjectBase
{
public:
	VObject();
	virtual ~VObject() override = default;

	virtual VClass* GetClass() const override;
	static VClass* StaticClass();
	
	void AddToRoot();
	void RemoveFromRoot();
	
	void AddFlags(EVObjectFlags InObjectFlags);
	void RemoveFlags(EVObjectFlags InObjectFlags);
	
	void MarkPendingKill();
	
	void SetOuter(VObject* InOuter);
	void SetName(const VString& InName);
	
	VObject* GetOuter() const;
	VObject* GetOuterMost() const;
	EVObjectFlags GetObjectFlags() const;
	VString GetName() const;
	VString GetPathName() const;
	
	bool HasAnyObjectFlags(EVObjectFlags InObjectFlags) const;
	bool HasAllObjectFlags(EVObjectFlags InObjectFlags) const;
	bool IsRootObject() const;
	bool IsPendingKill() const;

private:
	VObject* Outer;
	EVObjectFlags ObjectFlags;
	VString Name = VString::NullString;
};
}