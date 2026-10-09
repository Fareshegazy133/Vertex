// Copyright HNDRED GAMES. All Rights Reserved.

/**
 * Platform startup, declared without naming a backend so the Engine layer can call it.
 * It's Runtime-private, so only the Engine starts the platform.
 */

#pragma once

namespace Vertex
{
	/**
	 * Starts the platform backend and logs which backend it is.
	 *
	 * @warning Only one backend may define it. A second one, such as a Null backend, fails the link with LNK2005 until M1 picks the backend at startup.
	 * @note The raylib backend raises raylib's own log threshold to warnings, so raylib's INFO lines don't bypass Vertex::Log.
	 */
	void InitializePlatform();
}
