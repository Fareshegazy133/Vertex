// Vertex

#pragma once

#include "Core/Containers/Array.h"
#include "Core/Containers/Map.h"
#include "Core/Containers/Pair.h"
#include "Core/Types/String.h"
#include "Math/Rotator.h"
#include "Math/Transform.h"
#include "Math/Vector.h"

template <typename ElementType>
using VArray = VCore::VArray<ElementType>;

template <typename KeyType, typename ValueType>
using VMap = VCore::VMap<KeyType, ValueType>;

template <typename FirstType, typename SecondType>
using VPair = VCore::VPair<FirstType, SecondType>;

using VVector2 = VMath::VVector2;
using VVector2D = VMath::VVector2D;
using VVector3 = VMath::VVector3D;
using VVector3D = VMath::VVector3D;
using VVector = VMath::VVector;

using VRotator2 = VMath::VRotator2;
using VRotator2D = VMath::VRotator2D;
using VRotator3 = VMath::VRotator3D;
using VRotator3D = VMath::VRotator3D;
using VRotator = VMath::VRotator;

using VTransform2 = VMath::VTransform2D;
using VTransform2D = VMath::VTransform2D;
using VTransform3 = VMath::VTransform3D;
using VTransform3D = VMath::VTransform3D;
using VTransform = VMath::VTransform;

using VString = VCore::VString;
using NullPtr = std::nullptr_t;

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

enum : int8
{
    INDEX_NONE = VCore::INDEX_NONE
};