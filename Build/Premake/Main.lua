-- Build/Premake/Main.lua
--
-- premake entry point. Reads the pure-data descriptors (Vertex.lua and every <Name>.Module.lua)
-- and turns them into a Visual Studio solution. Everything premake-specific lives in this folder;
-- a future Vertex build tool would replace this folder and reuse the descriptors unchanged.
--
-- Usage (from the repo root; Scripts\Setup.bat runs both for you):
--   Build\Premake\Bin\Windows\premake5.exe --file=Build/Premake/Main.lua fetch
--   Build\Premake\Bin\Windows\premake5.exe --file=Build/Premake/Main.lua vs2022

-- Every path is absolute from here on. premake resolves relative paths against whichever
-- script is currently running, which silently breaks once one helper builds every module.
local Root = path.getabsolute("../..", _SCRIPT_DIR)

local Modules = dofile(path.join(_SCRIPT_DIR, "Modules.lua"))
local Fetch = dofile(path.join(_SCRIPT_DIR, "Fetch.lua"))

local Workspace = Modules.LoadData(path.join(Root, "Vertex.lua"))
local Descriptors, ByName = Modules.Discover(Root, Workspace.ModuleRoots)

Fetch.Register(Descriptors)

-- Never generate project files against missing or wrong third-party sources.
if _ACTION and _ACTION ~= "fetch" then
	Fetch.VerifyAll(Descriptors)
end

----------------------------------------------------------------------------------------------------
-- Workspace
----------------------------------------------------------------------------------------------------

local PlatformNames = { windows = "Win64", linux = "Linux", macosx = "Mac" }
local PlatformName = PlatformNames[os.target()] or os.target()

local ConfigNames = {}
for _, Config in ipairs(Workspace.Configurations) do
	table.insert(ConfigNames, Config.Name)
end

workspace(Workspace.Name)
	location(Root)
	architecture(Workspace.Architecture)
	configurations(ConfigNames)
	startproject(Workspace.StartModule)
	cppdialect(Workspace.CppStandard)

	-- One C runtime for every project, set once. Mixing runtimes causes LNK2038.
	staticruntime "Off"
	multiprocessorcompile "On"

	-- Executables for a configuration share one folder, so a future DLL can sit next to them.
	targetdir(path.join(Root, "Binaries", PlatformName .. "-%{cfg.buildcfg}"))
	objdir(path.join(Root, "Intermediate", "Build", "%{cfg.buildcfg}", "%{prj.name}"))

	filter "action:vs*"
		buildoptions { "/utf-8" }
	filter {}

	-- Map each configuration's intent (Vertex.lua) to compiler settings.
	for _, Config in ipairs(Workspace.Configurations) do
		filter("configurations:" .. Config.Name)
			defines { Config.Define, "VERTEX_ENABLE_ASSERTS=" .. (Config.Asserts and "1" or "0") }
			optimize(Config.Optimize and "Speed" or "Off")
			runtime(Config.Optimize and "Release" or "Debug")
			symbols(Config.Symbols and "On" or "Off")
	end
	filter {}

----------------------------------------------------------------------------------------------------
-- Projects
----------------------------------------------------------------------------------------------------

Modules.Declare(Root, Descriptors, ByName)
