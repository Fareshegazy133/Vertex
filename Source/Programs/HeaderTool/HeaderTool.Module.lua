-- HeaderTool: build-time code generator for reflection.
-- Depends on Core ONLY: Runtime will compile code that HeaderTool generates, so
-- depending on Runtime would create a build cycle.

return
{
	Name = "HeaderTool",
	Kind = "ConsoleApp",
	Group = "Programs",
	PrivateDependencies = { "Core" }
}
