-- raylib: the first platform/rendering backend (https://www.raylib.com, zlib license).
-- Only Runtime depends on it, privately, and only Runtime/Private/Platform/Raylib/** may include it.
--
-- To upgrade: change Tag and Commit together (git ls-remote --tags https://github.com/raysan5/raylib.git),
-- then run Scripts\Setup.bat --refetch.

return
{
	Name = "raylib",
	Kind = "StaticLib",
	Group = "ThirdParty",
	ThirdParty = true,
	Language = "C",
	CStandard = "C11",

	Source = {
		Git = "https://github.com/raysan5/raylib.git",
		Tag = "6.0",
		Commit = "dbc56a87da87d973a9c5baa4e7438a9d20121d28",
	},

	-- Relative to the fetched Source/ folder.
	Files = {
		"src/rcore.c",
		"src/rshapes.c",
		"src/rtextures.c",
		"src/rtext.c",
		"src/rmodels.c",
		"src/raudio.c",
		"src/rglfw.c",
	},
	PublicIncludeDirs = { "src" },
	PrivateIncludeDirs = { "src/external/glfw/include" },

	PrivateDefines = {
		"PLATFORM_DESKTOP_GLFW",
		"GRAPHICS_API_OPENGL_33",
		"_CRT_SECURE_NO_WARNINGS",
	},

	-- Whatever links raylib must also link these.
	SystemLibraries = {
		Windows = { "opengl32", "gdi32", "winmm", "shell32" }, -- matches raylib's own src/Makefile
	},
}
