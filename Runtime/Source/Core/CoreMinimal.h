// Vertex

#pragma once

#include "Core/CoreAliases.h"
#include "Core/Memory/ObjectPtr.h"
#include "Core/Memory/UniquePtr.h"
#include "Core/Object/Object.h"

template <typename ElementType> requires std::derived_from<ElementType, VCore::VObject>
using VObjectPtr = VCore::VObjectPtr<ElementType>;

template <typename ElementType> requires std::derived_from<ElementType, VCore::VObject>
using VUniquePtr = VCore::VUniquePtr<ElementType>;

using VObject = VCore::VObject;
using VCore::MakeUnique;