-- Runtime: platform, input, rendering, scene, engine loop.
-- The only module that knows raylib exists. raylib is a PRIVATE dependency, so its
-- headers never reach Runtime's consumers (Editor cannot #include "raylib.h").

return
{
	Name = "Runtime",
	Kind = "StaticLib",
	Group = "Engine",
	PublicDependencies = { "Core" },
	PrivateDependencies = { "raylib" }
}
