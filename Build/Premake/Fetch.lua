-- Build/Premake/Fetch.lua
--
-- Fetches third-party sources at the exact version pinned in their descriptor:
--
--   Source = { Git = "<https url>", Tag = "<release tag>", Commit = "<40-char SHA>" }
--
-- The Tag says which release we want. The Commit is the integrity check: tags can be moved
-- upstream, commits can't, so sources whose HEAD isn't the pinned Commit are rejected.
-- Sources land in ThirdParty/<Name>/Source/ (gitignored).
--
-- Safety rules:
--   * An up-to-date clone is verified locally, with no network access.
--   * A mismatched clone fails loudly. Nothing is ever deleted: the folder may hold local patches.
--   * --refetch moves an existing clone to the pinned commit, and refuses if it has local changes.

local Fetch = {}

local function Fail(Message)
	error("[Fetch] " .. Message, 0)
end

local function Quote(Text)
	return '"' .. Text .. '"'
end

-- Runs a shell command; returns its trimmed output and whether it exited with 0.
-- (premake's os.outputof runs the whole command through path.normalize, which mangles URLs.)
local function Run(Command)
	local Pipe = io.popen(Command .. " 2>&1")
	local Output = Pipe:read("*a") or ""
	local Succeeded = Pipe:close()
	return (Output:gsub("[\r\n]+$", "")), Succeeded == true
end

local function SourceDir(Module)
	return path.join(Module.Dir, "Source")
end

local function Short(Commit)
	return Commit:sub(1, 10)
end

local function ValidateManifest(Module)
	local Source = Module.Source
	if type(Source) ~= "table" or not Source.Git or not Source.Tag or not Source.Commit then
		Fail(Module.Name .. ": Source must be { Git = ..., Tag = ..., Commit = ... }.")
	end
	if not Source.Git:find("^https://") then
		Fail(Module.Name .. ": Source.Git must be an https:// URL.")
	end
	if #Source.Commit ~= 40 or not Source.Commit:find("^%x+$") then
		Fail(Module.Name .. ": Source.Commit must be a full 40-character commit SHA.")
	end
end

local function RequireGit()
	local _, Found = Run("git --version")
	if not Found then
		Fail("git was not found on PATH. Install Git for Windows, then run Scripts\\Setup.bat again.")
	end
end

-- Returns "Ok", "Missing", or "Mismatch", plus the commit actually checked out.
local function Status(Module)
	local Dir = SourceDir(Module)
	if not os.isdir(Dir) then
		return "Missing"
	end
	local Head, IsClone = Run("git -C " .. Quote(Dir) .. " rev-parse HEAD")
	if not IsClone then
		return "Mismatch", "<not a git clone>"
	end
	if Head == Module.Source.Commit then
		return "Ok", Head
	end
	return "Mismatch", Head
end

local function MismatchMessage(Module, Head)
	return ("%s sources are at %s, but %s.Module.lua pins %s (%s).\n"
		.. "  Run Scripts\\Setup.bat --refetch to move them to the pinned commit.\n"
		.. "  Nothing was changed or deleted.")
		:format(Module.Name, Head, Module.Name, Module.Source.Commit, Module.Source.Tag)
end

local function FetchModule(Module)
	local Source = Module.Source
	local Dir = SourceDir(Module)
	local State, Head = Status(Module)

	if State == "Ok" then
		print(("  %-12s up to date (%s @ %s)"):format(Module.Name, Source.Tag, Short(Source.Commit)))
		return
	end

	if State == "Missing" then
		print(("  %-12s cloning %s ..."):format(Module.Name, Source.Tag))
		local Output, Cloned = Run(("git -c advice.detachedHead=false clone --depth 1 --branch %s %s %s")
			:format(Source.Tag, Source.Git, Quote(Dir)))
		if not Cloned then
			Fail("Cloning " .. Module.Name .. " failed:\n" .. Output)
		end
	elseif _OPTIONS["refetch"] then
		local Changes = Run("git -C " .. Quote(Dir) .. " status --porcelain")
		if Changes ~= "" then
			Fail(Module.Name .. " sources have local changes. Commit, stash, or remove them first:\n" .. Changes)
		end
		print(("  %-12s moving to %s ..."):format(Module.Name, Source.Tag))
		local Output, Fetched = Run(("git -C %s fetch --depth 1 --force %s tag %s")
			:format(Quote(Dir), Source.Git, Source.Tag))
		if not Fetched then
			Fail("Fetching " .. Module.Name .. " failed:\n" .. Output)
		end
		Run(("git -c advice.detachedHead=false -C %s checkout --detach %s"):format(Quote(Dir), Source.Commit))
	else
		Fail(MismatchMessage(Module, Head))
	end

	-- The integrity check: whatever we cloned or checked out must be exactly the pinned commit.
	local NewState, NewHead = Status(Module)
	if NewState ~= "Ok" then
		Fail(("%s: tag %q gave commit %s, but the pinned commit is %s.\n"
			.. "  The upstream tag may have been moved. Don't use these sources until you know why.")
			:format(Module.Name, Source.Tag, tostring(NewHead), Source.Commit))
	end
	print(("  %-12s ok (%s @ %s)"):format(Module.Name, Source.Tag, Short(Source.Commit)))
end

-- Fails unless every third-party module is present at its pinned commit.
-- Main.lua runs this before generating project files.
function Fetch.VerifyAll(Descriptors)
	for _, Module in ipairs(Descriptors) do
		if Module.Source then
			local State, Head = Status(Module)
			if State == "Missing" then
				Fail(Module.Name .. " sources are missing. Run Scripts\\Setup.bat first.")
			elseif State == "Mismatch" then
				Fail(MismatchMessage(Module, Head))
			end
		end
	end
end

-- Registers the `fetch` action and its `--refetch` option.
function Fetch.Register(Descriptors)
	for _, Module in ipairs(Descriptors) do
		if Module.Source then
			ValidateManifest(Module)
		end
	end

	newoption {
		trigger = "refetch",
		description = "fetch: move existing third-party clones to their pinned commit (refuses if they have local changes)",
	}

	newaction {
		trigger = "fetch",
		description = "Fetch third-party sources at the tag + commit pinned in their *.Module.lua",
		execute = function()
			RequireGit()
			print("Fetching third-party sources...")
			for _, Module in ipairs(Descriptors) do
				if Module.Source then
					FetchModule(Module)
				end
			end
		end,
	}
end

return Fetch
