// Copyright HNDRED GAMES. All Rights Reserved.

#pragma once

#include <cstdint>
#include <string_view>

enum class ELogLevel : std::uint8_t
{
	Info,
	Warning,
	Error
};

namespace Vertex
{
	CORE_API void Log(const ELogLevel LogLevel, const std::string_view Message);
}
