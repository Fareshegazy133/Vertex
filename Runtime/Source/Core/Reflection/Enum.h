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
		VName Name;
		int64 Value;
	};

public:
	explicit VEnum(const VName& InName = NAME_None);
	virtual ~VEnum() override = default;

	void AddEnumerator(const VName& Name, const int64 Value);

	const Enumerator* FindEnumerator(const VName& EnumeratorName) const;
	const Enumerator* FindEnumerator(const int64 Value) const;
	
	const Enumerator* GetEnumerator(const TSize Index) const;
	TSize GetEnumeratorCount() const;

private:
	VArray<Enumerator> Enumerators;
};
}