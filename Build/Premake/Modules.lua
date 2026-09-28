-- Build/Premake/Modules.lua
--
-- Turns pure-data module descriptors (<Name>.Module.lua) into premake projects.
--
-- The rules this file enforces (see CLAUDE.md, "Module graph"):
--   * PUBLIC dependency:  its headers are visible to me AND to everyone who depends on me.
--   * PRIVATE dependency: its headers are visible to me only.
--   * Link requirements always flow up to the final executable, because a static .lib
--     can't carry its own dependencies. Include paths do not flow up.
--   * Unknown dependencies, dependency cycles, and depending on an application are errors.
--
-- Descriptor fields (all optional unless noted):
--   Name                  (required) must match the file name: Core.Module.lua -> "Core"
--   Kind                  (required) "StaticLib" | "ConsoleApp" | "WindowedApp"
--   Group                 solution folder, e.g. "Engine"
--   PublicDependencies    module names whose headers my consumers also see
--   PrivateDependencies   module names only I see
--   PublicDefines         defines for me and everyone who sees my headers
--   PrivateDefines        defines for me only
--   SystemLibraries       { Windows = { "winmm", ... } } -- OS libraries the final executable must link
--
-- Third-party descriptors additionally set:
--   ThirdParty = true     warnings off, headers treated as external, no <NAME>_API macro
--   Language, CStandard   e.g. "C", "C11"
--   Source                { Git, Tag, Commit } -- see Fetch.lua
--   Files                 source files, relative to the fetched Source/ folder
--   PublicIncludeDirs     relative to Source/
--   PrivateIncludeDirs    relative to Source/
--
-- Vertex modules use conventions instead of file lists:
--   Public/   headers other modules may include (the include root)
--   Private/  .cpp files and internal headers

local Modules = {}

local ValidKinds = { StaticLib = true, ConsoleApp = true, WindowedApp = true }
local AppKinds = { ConsoleApp = true, WindowedApp = true }

-- Descriptor platform names -> premake filter terms.
local SystemFilters = { Windows = "system:windows", Linux = "system:linux", Mac = "system:macosx" }

local function Fail(Message)
	error("[Modules] " .. Message, 0)
end

-- Appends Item to List unless it's already there. Keeps output order deterministic.
local function AddUnique(List, Seen, Item)
	if not Seen[Item] then
		Seen[Item] = true
		table.insert(List, Item)
	end
end

local function AllDependencies(Module)
	local Result = {}
	for _, Name in ipairs(Module.PublicDependencies) do table.insert(Result, Name) end
	for _, Name in ipairs(Module.PrivateDependencies) do table.insert(Result, Name) end
	return Result
end

----------------------------------------------------------------------------------------------------
-- Loading
----------------------------------------------------------------------------------------------------

-- Loads a pure-data Lua file in a sandbox: the file runs with an EMPTY environment, so it can
-- only build and return a table. Any premake call (or any global at all) is an error.
-- This is what keeps descriptors readable by a future, non-premake build tool.
function Modules.LoadData(File)
	local Handle = io.open(File, "r")
	if not Handle then
		Fail("Can't open " .. File)
	end
	local Source = Handle:read("*a")
	Handle:close()

	local Chunk, SyntaxError = load(Source, "@" .. File, "t", {})
	if not Chunk then
		Fail(SyntaxError)
	end

	local Succeeded, Result = pcall(Chunk)
	if not Succeeded then
		Fail(Result .. "\n  (Descriptors are pure data: they can't call functions or use globals.)")
	end
	if type(Result) ~= "table" then
		Fail(File .. " must return a table.")
	end
	return Result
end

local function LoadDescriptor(File)
	local Module = Modules.LoadData(File)

	local ExpectedName = path.getbasename(File):gsub("%.Module$", "")
	if Module.Name ~= ExpectedName then
		Fail(("%s declares Name = %q, but its file name says %q."):format(File, tostring(Module.Name), ExpectedName))
	end
	if not ValidKinds[Module.Kind] then
		Fail(("%s: Kind %q must be StaticLib, ConsoleApp, or WindowedApp."):format(File, tostring(Module.Kind)))
	end

	Module.File = File
	Module.Dir = path.getdirectory(File)
	Module.PublicDependencies = Module.PublicDependencies or {}
	Module.PrivateDependencies = Module.PrivateDependencies or {}
	Module.PublicDefines = Module.PublicDefines or {}
	Module.PrivateDefines = Module.PrivateDefines or {}
	Module.SystemLibraries = Module.SystemLibraries or {}
	return Module
end

----------------------------------------------------------------------------------------------------
-- Validation
----------------------------------------------------------------------------------------------------

local function Validate(Descriptors, ByName)
	for _, Module in ipairs(Descriptors) do
		for _, DependencyName in ipairs(AllDependencies(Module)) do
			local Dependency = ByName[DependencyName]
			if not Dependency then
				Fail(("%s depends on %q, but no module has that name.\n  (Looked for %s.Module.lua under every ModuleRoot in Vertex.lua.)")
					:format(Module.Name, DependencyName, DependencyName))
			end
			if Dependency == Module then
				Fail(Module.Name .. " depends on itself.")
			end
			if AppKinds[Dependency.Kind] then
				Fail(("%s depends on %s, which is an application. Only libraries can be dependencies.")
					:format(Module.Name, DependencyName))
			end
		end
	end

	-- Cycle detection with a depth-first search. "Visiting" means "on the current path":
	-- reaching a Visiting module again means the path has looped back on itself.
	local State = {}
	local Path = {}

	local function Visit(Module)
		if State[Module.Name] == "Done" then
			return
		end
		if State[Module.Name] == "Visiting" then
			local Cycle = {}
			local Collecting = false
			for _, Name in ipairs(Path) do
				Collecting = Collecting or Name == Module.Name
				if Collecting then table.insert(Cycle, Name) end
			end
			table.insert(Cycle, Module.Name)
			Fail("Dependency cycle: " .. table.concat(Cycle, " -> "))
		end

		State[Module.Name] = "Visiting"
		table.insert(Path, Module.Name)
		for _, DependencyName in ipairs(AllDependencies(Module)) do
			Visit(ByName[DependencyName])
		end
		table.remove(Path)
		State[Module.Name] = "Done"
	end

	for _, Module in ipairs(Descriptors) do
		Visit(Module)
	end
end

----------------------------------------------------------------------------------------------------
-- Discovery
----------------------------------------------------------------------------------------------------

-- Finds, loads, and validates every <Name>.Module.lua under the given roots.
-- Returns the descriptors sorted by Name (so generation is deterministic) and a Name -> descriptor map.
function Modules.Discover(Root, ModuleRoots)
	local Descriptors = {}
	local ByName = {}

	for _, ModuleRoot in ipairs(ModuleRoots) do
		for _, File in ipairs(os.matchfiles(path.join(Root, ModuleRoot, "**.Module.lua"))) do
			-- Never look inside fetched third-party sources (ThirdParty/<Name>/Source/...).
			if not File:find("/ThirdParty/[^/]+/Source/") then
				local Module = LoadDescriptor(File)
				if ByName[Module.Name] then
					Fail(("Two modules are named %q:\n  %s\n  %s"):format(Module.Name, ByName[Module.Name].File, File))
				end
				ByName[Module.Name] = Module
				table.insert(Descriptors, Module)
			end
		end
	end

	table.sort(Descriptors, function(A, B) return A.Name < B.Name end)
	Validate(Descriptors, ByName)
	return Descriptors, ByName
end

----------------------------------------------------------------------------------------------------
-- Graph queries
----------------------------------------------------------------------------------------------------

-- The modules whose headers `Module` exposes to its consumers: itself, plus (recursively)
-- its PUBLIC dependencies. Private dependencies stop the walk; that's the whole point.
local function CollectPublicClosure(Module, ByName, List, Seen)
	if Seen[Module.Name] then
		return
	end
	AddUnique(List, Seen, Module.Name)
	for _, Name in ipairs(Module.PublicDependencies) do
		CollectPublicClosure(ByName[Name], ByName, List, Seen)
	end
end

-- Every module whose headers `Module` can see: its own, plus the public closure of each
-- dependency (public or private).
local function VisibleModules(Module, ByName)
	local List, Seen = {}, {}
	AddUnique(List, Seen, Module.Name)
	for _, Name in ipairs(AllDependencies(Module)) do
		CollectPublicClosure(ByName[Name], ByName, List, Seen)
	end
	return List
end

-- Every library an application must link: all dependencies, transitively, through BOTH
-- public and private edges. Ordered dependents-before-dependencies, which GNU-style linkers
-- need (MSVC doesn't care, but Linux will).
local function LinkClosure(Module, ByName)
	local PostOrder, Seen = {}, {}
	local function Visit(Current)
		for _, Name in ipairs(AllDependencies(Current)) do
			if not Seen[Name] then
				Seen[Name] = true
				Visit(ByName[Name])
				table.insert(PostOrder, Name)
			end
		end
	end
	Visit(Module)

	local Ordered = {}
	for Index = #PostOrder, 1, -1 do
		table.insert(Ordered, PostOrder[Index])
	end
	return Ordered
end

----------------------------------------------------------------------------------------------------
-- Per-module include directories
----------------------------------------------------------------------------------------------------

local function SourceDir(Module)
	return path.join(Module.Dir, "Source")
end

local function ResolveAll(BaseDir, RelativePaths)
	local Result = {}
	for _, Relative in ipairs(RelativePaths or {}) do
		table.insert(Result, path.join(BaseDir, Relative))
	end
	return Result
end

local function PublicIncludeDirs(Module)
	if Module.ThirdParty then
		return ResolveAll(SourceDir(Module), Module.PublicIncludeDirs)
	end
	return { path.join(Module.Dir, "Public") }
end

local function PrivateIncludeDirs(Module)
	if Module.ThirdParty then
		return ResolveAll(SourceDir(Module), Module.PrivateIncludeDirs)
	end
	return { path.join(Module.Dir, "Private") }
end

-- CORE_API, RUNTIME_API, ... Defined empty while modules are static libraries.
-- Switching a module to a DLL later means changing this definition, not every header.
local function ApiMacro(Module)
	return Module.Name:upper() .. "_API="
end

----------------------------------------------------------------------------------------------------
-- Project emission
----------------------------------------------------------------------------------------------------

local function DeclareVertexModule(Module)
	language "C++"
	targetname("Vertex" .. Module.Name)

	files {
		path.join(Module.Dir, "Public/**.h"),
		path.join(Module.Dir, "Public/**.inl"),
		path.join(Module.Dir, "Private/**.h"),
		path.join(Module.Dir, "Private/**.inl"),
		path.join(Module.Dir, "Private/**.cpp"),
	}

	-- Our code: strict. /W4, warnings are errors, standard-conforming C++.
	warnings "Extra"
	fatalwarnings { "All" }
	conformancemode(true)
	usestandardpreprocessor "On"
	filter "action:vs*"
		buildoptions { "/Zc:__cplusplus" }
	filter {}

	-- Their code: third-party headers we include are "external", so their warnings
	-- can't break our /WX build.
	externalwarnings "Off"
end

local function DeclareThirdPartyModule(Module)
	language(Module.Language or "C++")
	if Module.CStandard then
		cdialect(Module.CStandard)
	end

	if not Module.Files or #Module.Files == 0 then
		Fail(Module.Name .. " is a third-party module, so it must list its Files.")
	end
	files(ResolveAll(SourceDir(Module), Module.Files))

	-- We don't own this code, so its warnings are not our signal. That includes the system
	-- headers it pulls in, which MSVC classifies as "external" and warns about separately
	-- (e.g. GLFW and <windows.h> both define APIENTRY: C4005).
	warnings "Off"
	externalwarnings "Off"
end

local function DeclareIncludesAndDefines(Module, ByName)
	-- My own headers.
	includedirs(PublicIncludeDirs(Module))
	includedirs(PrivateIncludeDirs(Module))
	defines(Module.PrivateDefines)

	-- Everything I can see: my own public surface, plus each dependency's public closure.
	for _, Name in ipairs(VisibleModules(Module, ByName)) do
		local Visible = ByName[Name]
		if Visible ~= Module then
			if Visible.ThirdParty then
				externalincludedirs(PublicIncludeDirs(Visible))
			else
				includedirs(PublicIncludeDirs(Visible))
			end
		end
		defines(Visible.PublicDefines)
		if not Visible.ThirdParty and Visible.Kind == "StaticLib" then
			defines { ApiMacro(Visible) }
		end
	end
end

local function DeclareLinks(Module, ByName)
	local Libraries = LinkClosure(Module, ByName)
	links(Libraries)

	-- OS libraries requested by me or by anything I link.
	local Participants = { Module }
	for _, Name in ipairs(Libraries) do
		table.insert(Participants, ByName[Name])
	end
	for _, Participant in ipairs(Participants) do
		for Platform, SystemLibraries in pairs(Participant.SystemLibraries) do
			local Filter = SystemFilters[Platform]
			if not Filter then
				Fail(("%s: unknown platform %q in SystemLibraries (use Windows, Linux, or Mac)."):format(Participant.Name, Platform))
			end
			filter(Filter)
				links(SystemLibraries)
			filter {}
		end
	end
end

-- Emits one premake project per descriptor. Call after the workspace is declared.
function Modules.Declare(Root, Descriptors, ByName)
	for _, Module in ipairs(Descriptors) do
		group(Module.Group or "")
		project(Module.Name)
			kind(Module.Kind)
			location(path.join(Root, "Intermediate", "ProjectFiles"))

			-- Show the descriptor in the IDE so it's one click away.
			files { Module.File }

			if Module.ThirdParty then
				DeclareThirdPartyModule(Module)
			else
				DeclareVertexModule(Module)
			end

			-- Libraries are build products, not things you run: keep them out of Binaries/.
			if Module.Kind == "StaticLib" then
				targetdir(path.join(Root, "Intermediate", "Build", "%{cfg.buildcfg}", "%{prj.name}"))
			end

			DeclareIncludesAndDefines(Module, ByName)

			-- Static libraries never link anything themselves; the executable links everything.
			-- That also lets independent libraries compile in parallel.
			if AppKinds[Module.Kind] then
				DeclareLinks(Module, ByName)
			end
	end
	group ""
end

return Modules
