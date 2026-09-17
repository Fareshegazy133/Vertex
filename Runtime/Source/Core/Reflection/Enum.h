// Vertex

#pragma once

#include "Core/Containers/Array.h"
#include "Core/Reflection/Field.h"

namespace VCore
{
class VEnum : public VField
{
public:
	struct Enumerator
	{
		VString Name;
		int64 Value;
	};

public:
	explicit VEnum(const VString& InName = VString::NullString);
	virtual ~VEnum() override = default;

	void AddEnumerator(const VString& Name, const int64 Value);

	const Enumerator* FindEnumerator(const VString& EnumeratorName) const;
	const Enumerator* FindEnumerator(const int64 Value) const;
	
	const Enumerator* GetEnumerator(const TSize Index) const;
	TSize GetEnumeratorCount() const;

private:
	VArray<Enumerator> Enumerators;
};
}