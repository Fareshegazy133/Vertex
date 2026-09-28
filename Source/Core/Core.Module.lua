-- Core: foundation types, containers, memory, strings/names, math, logging, asserts.
-- Rule: no platform, windowing, rendering, or generated (reflection) code.
-- Every other Vertex module may depend on Core; Core depends on nothing.

return
{
	Name = "Core",
	Kind = "StaticLib",
	Group = "Engine"
}
