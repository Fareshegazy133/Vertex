-- doctest: the unit-test framework (https://github.com/doctest/doctest, MIT license).
-- Only test executables depend on it. A module that ships must never depend on doctest.
--
-- Built from upstream's own doctest/doctest.cpp, which compiles the framework once, here.
-- Each test executable supplies its own main(), so it can set up whatever its tests need first.
--
-- A DOCTEST_CONFIG_* define that changes the framework's types or behavior (NO_EXCEPTIONS, for
-- example) belongs in PublicDefines: the library and every test must compile with the same set.
--
-- To upgrade: change Tag and Commit together (git ls-remote --tags https://github.com/doctest/doctest.git),
-- then run Scripts\Setup.bat --refetch.

return
{
	Name = "doctest",
	Kind = "StaticLib",
	Group = "ThirdParty",
	ThirdParty = true,
	Language = "C++",

	Source =
	{
		Git = "https://github.com/doctest/doctest.git",
		Tag = "v2.5.3",
		Commit = "2d0a9359a60c51affe2a9bebb1be1dca47868151",
	},

	-- Relative to the fetched Source/ folder.
	Files =
	{
		"doctest/doctest.cpp",
	},

	-- The repository root, so includes name the library: #include "doctest/doctest.h".
	PublicIncludeDirs = { "." },
}
