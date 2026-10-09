// Copyright HNDRED GAMES. All Rights Reserved.

#include "Core/Logging/Log.h"
#include <cstdlib>
#include <span>

namespace
{
	[[nodiscard]] int Run(const std::span<const char* const> Arguments)
	{
		if (Arguments.size() != 1)
		{
			Vertex::Log(ELogLevel::Error, "Expected one argument. Usage: VertexHeaderTool <ManifestPath>");
			return EXIT_FAILURE;
		}

		Vertex::Log(ELogLevel::Info, "Nothing to generate yet: reflection arrives in M3");
		return EXIT_SUCCESS;
	}
}

int main(const int ArgC, char** const ArgV)
{
	const std::span<const char* const> AllArguments(ArgV, static_cast<std::size_t>(ArgC));
	const std::span<const char* const> Arguments = AllArguments.empty() ? AllArguments : AllArguments.subspan(1);
	return Run(Arguments);
}
