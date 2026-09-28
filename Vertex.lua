-- Vertex workspace descriptor.
--
-- Pure data: a future Vertex build tool must be able to read this file unchanged.
-- It is loaded in a sandbox with no globals, so calling any premake API here is an error.
-- Build/Premake/Main.lua turns it into a Visual Studio solution.

return {
	Name = "Vertex",
	Architecture = "x64",
	CppStandard = "C++20",
	StartModule = "Editor",

	-- Folders scanned (recursively) for <Name>.Module.lua descriptors, relative to this file.
	ModuleRoots = { "Source", "ThirdParty" },

	-- What each configuration *means*. The build layer maps these intents to compiler flags.
	Configurations = {
		{ Name = "Debug",       Define = "VERTEX_DEBUG",       Optimize = false, Symbols = true, Asserts = true  },
		{ Name = "Development", Define = "VERTEX_DEVELOPMENT", Optimize = true,  Symbols = true, Asserts = true  },
		-- Shipping keeps symbols: the PDBs are never shipped, but without them
		-- crash dumps from players' machines can't be read.
		{ Name = "Shipping",    Define = "VERTEX_SHIPPING",    Optimize = true,  Symbols = true, Asserts = false },
	},
}
