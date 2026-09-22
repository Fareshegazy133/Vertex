// Vertex

#include <utility>

#include "Core/Reflection/Enum.h"

namespace VCore
{
VEnum::VEnum(const VName& InName)
{
	FieldName = InName;
}

void VEnum::AddEnumerator(const VName& Name, const int64 Value)
{
	Enumerator Enumerator;
	Enumerator.Name = Name;
	Enumerator.Value = Value;
	Enumerators.Add(Enumerator);
}

const VEnum::Enumerator* VEnum::FindEnumerator(const VName& EnumeratorName) const
{
	if (!EnumeratorName.IsValid()) return nullptr;

	for (const auto& Enumerator : Enumerators)
	{
		if (Enumerator.Name == EnumeratorName) return &Enumerator;
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