-- Editor: the editor executable. Depends on Runtime only; it sees Core through
-- Runtime's public dependency and never sees raylib.

return
{
	Name = "Editor",
	Kind = "ConsoleApp",
	Group = "Engine",
	PrivateDependencies = { "Runtime" },
}
