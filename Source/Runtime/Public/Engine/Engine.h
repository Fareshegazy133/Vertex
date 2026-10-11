// Copyright HNDRED GAMES. All Rights Reserved.

/** Engine startup: the one call an application makes to bring up Runtime. M2's engine loop replaces it. */

#pragma once

namespace Vertex
{
	/**
	 * Starts every Runtime layer, from the bottom up. Today that's the platform backend, which logs its name.
	 *
	 * @warning Call it before any other Runtime API. Until it returns, no Runtime layer has started.
	 * @note There's no matching shutdown yet, because nothing it starts holds a resource. M2's engine loop brings the full lifecycle.
	 */
	RUNTIME_API void InitializeEngine();
}
