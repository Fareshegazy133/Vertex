-- CoreTests: Core's unit tests, as one console executable built on doctest.
-- Depends on Core and doctest only, so no test can reach Runtime, the platform, or raylib.
--
-- Tests live in this executable, never in Core: a linker drops any .obj from a static library
-- that nothing references, and a self-registering test is exactly such an .obj.
--
-- Run from the repo root: Binaries\Win64-<Config>\VertexCoreTests.exe

return
{
	Name = "CoreTests",
	Kind = "ConsoleApp",
	Group = "Tests",
	PrivateDependencies = { "Core", "doctest" },
}
