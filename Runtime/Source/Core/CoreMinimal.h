// Vertex

#pragma once

#include "DataStructs/Array.h"
#include "Memory/ObjectPtr.h"
#include "Memory/UniquePtr.h"
#include "Types/String.h"

template<typename ElementType>
using VArray = VCore::VArray<ElementType>;

template<typename ElementType> requires std::derived_from<ElementType, VObject>
using VObjectPtr = VCore::VObjectPtr<ElementType>;

template<typename ElementType> requires std::derived_from<ElementType, VObject>
using VUniquePtr = VCore::VUniquePtr<ElementType>;

using VCore::MakeUnique;

using VString = VCore::VString;

using int8 = VCore::int8;
using uint8 = VCore::uint8;

using int16 = VCore::int16;
using uint16 = VCore::uint16;

using int32 = VCore::int32;
using uint32 = VCore::uint32;

using int64 = VCore::int64;
using uint64 = VCore::uint64;

using float32 = VCore::float32;
using float64 = VCore::float64;

using intptr = VCore::intptr;
using uintptr = VCore::uintptr;

using char8 = VCore::char8;
using char16 = VCore::char16;
using char32 = VCore::char32;

enum : int8 { INDEX_NONE = VCore::INDEX_NONE };