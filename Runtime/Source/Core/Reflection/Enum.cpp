// Vertex

#include <utility>

#include "Core/Reflection/Enum.h"

namespace VCore
{
VEnum::VEnum(const VString& InName)
{
	FieldName = InName;
}

void VEnum::AddEnumerator(const VString& Name, const int64 Value)
{
	Enumerators.Add({.Name = Name,.Value = Value});
}

const VEnum::Enumerator* VEnum::FindEnumerator(const VString& EnumeratorName) const
{
	if (!EnumeratorName) return nullptr;

	for (const auto& Enumerator : Enumerators)
	{
		if (Enumerator.Name && std::strcmp(Enumerator.Name, EnumeratorName) == 0) return &Enumerator;
	}

	return nullptr;
}

const VEnum::Enumerator* VEnum::FindEnumerator(const int64 Value) const
{
	for (const auto& Enumerator : Enumerators)
	{
		if (Enumerator.Value == Value) return &Enumerator;
	}

	return nullptr;
}

const VEnum::Enumerator* VEnum::GetEnumerator(const TSize Index) const
{
	if (std::cmp_greater_equal(Index, Enumerators.Num())) return nullptr;
	return &Enumerators[Index];
}

TSize VEnum::GetEnumeratorCount() const
{
	return Enumerators.Num();
}
}