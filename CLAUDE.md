# Vertex — Project Contract

Vertex is a C++20 mini engine. raylib is the first backend and must stay replaceable. This file is the contract: if a change breaks a rule here, stop and say which rule and why.

Working mode: Fares is learning engine architecture by building it.
- **Claude writes the build setup:** everything under `Build/`, `Scripts/`, `Vertex.lua`, and every `*.Module.lua`. Also docs and mechanical edits. After writing, walk Fares through what each file does and why.
- **Fares writes all engine C++**, walked through one card at a time (see the user-level CLAUDE.md, Mentorship Mode rule 7).

## Module graph

```
raylib (ThirdParty, C) ←─private── Runtime ──public──→ Core
                                     ↑                   ↑
                                  Editor            HeaderTool
```

- **Core**: types, containers, memory, strings/names, math, logging, asserts. **No** platform, windowing, rendering, or generated (reflection) code.
- **Runtime**: platform, input, rendering, scene, engine loop. The only module that depends on raylib, and **privately**.
- **Editor**: application. Depends on Runtime only.
- **HeaderTool**: application. Depends on **Core only**. It generates code that Runtime compiles, so depending on Runtime would create a build cycle.
- A **public** dependency's headers are visible to my consumers; a **private** one's are visible only to me. Link requirements still flow up to the executable; include paths do not.
- Runtime never depends on Editor.

## The raylib rule

1. Only `Source/Runtime/Private/Platform/Raylib/**` may include `raylib.h`, `rlgl.h`, or `raymath.h`.
2. No raylib type, enum, or macro may appear in any `Public/` header. Vertex owns its vocabulary (`VKey`, `VColor`, `VWindowDesc`, …) and the backend translates.
3. `raylib.h` and `<windows.h>` define colliding names (`CloseWindow`, `DrawText`, `LoadImage`, `Rectangle`, …). They must never meet in one translation unit.

## Runtime layering (folders are modules-in-waiting)

```
Platform/  → Core
Input/     → Core, Platform
Render/    → Core, Platform
Scene/     → Core, Input, Render
Engine/    → everything above
```

A folder never includes a folder above it. Promote a folder to its own module **only** when a trigger fires:
- a target needs a subset (for example, a dedicated server without Render);
- a third-party dependency needs isolation (for example, a second GPU backend);
- runtime loading (plugins, hot reload);
- measured build-time pain.

## Build system: data vs. how

- `Vertex.lua` (workspace) and every `*.Module.lua` are **pure data**: they `return` a table and never call premake APIs. A future Vertex build tool must be able to read them unchanged.
- Everything premake-specific lives in `Build/Premake/` (`Main.lua`, `Modules.lua`, `Fetch.lua`).
- premake is pinned (`Build/Premake/Bin/Windows/premake5.exe`, v5.0.0-beta8). Don't upgrade it casually.
- Third-party sources are fetched by **tag and verified commit SHA** declared in their descriptor, never a branch. A commit mismatch fails loudly and never auto-deletes.
- Configurations: `Debug`, `Development`, `Shipping`. Defines are prefixed (`VERTEX_DEBUG`, …). Shipping still produces PDBs.
- Vertex modules build at `/W4` with warnings as errors, plus C4062 (an enum value missing from a `switch` with no `default:`). ThirdParty builds with warnings off.
- Switches over an enum don't use `default:`, so C4062 flags every switch that forgets a newly added value.
- Generated files go to `Intermediate/ProjectFiles/`, binaries to `Binaries/`. Neither is committed.

## Commands

| Task | Command (from the repo root) |
|---|---|
| Fetch third-party sources and generate `Vertex.sln` | `Scripts\Setup.bat` |
| After bumping a third-party version | `Scripts\Setup.bat --refetch` |
| Delete generated output (keeps fetched sources) | `Scripts\Clean.bat` |
| Build everything | `& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" Vertex.sln -m -p:Configuration=<Debug\|Development\|Shipping> -p:Platform=x64` |
| Build one project | Same, but pass `Intermediate\ProjectFiles\<Module>.vcxproj` instead of the `.sln`. Solution folders make `-t:<Name>` awkward. |

- Solution project names are the module names (`Core`, `Runtime`, …). Output files are prefixed: `VertexEditor.exe`, `VertexCore.lib`.
- Executables go to `Binaries\Win64-<Config>\`, libraries to `Intermediate\Build\<Config>\<Module>\`.
- Regenerate (`Setup.bat`) after adding, removing, or renaming source files. New files are only picked up at generation time.
- Create new source files on disk under `Source/<Module>/`, never through the IDE's project tree. The generated projects live in `Intermediate/ProjectFiles/`, so an IDE "Add New File" lands there. That folder is git-ignored and deleted by `Clean.bat`.

## Defines available to C++

| Define | Meaning |
|---|---|
| `VERTEX_DEBUG` / `VERTEX_DEVELOPMENT` / `VERTEX_SHIPPING` | Exactly one is defined, per configuration |
| `VERTEX_ENABLE_ASSERTS` | `1` in Debug and Development, `0` in Shipping |
| `CORE_API`, `RUNTIME_API` | Export markers for a module's public classes and functions. Defined **empty** while modules are static libraries; they become `__declspec(dllexport/dllimport)` if a module ever becomes a DLL. Use them on public API from day one. |

## Source layout per module

```
Source/<Module>/
  <Module>.Module.lua
  Public/    headers other modules may include (the include root)
  Private/   .cpp files and internal headers
```

Core's public headers live under `Public/Core/…`, so includes read `#include "Core/Containers/Array.h"`. The `Core/` prefix tells every consumer which module a header comes from, and keeps Core's generic folder names (`Containers`, `Math`, `Logging`) from colliding with anyone else's.

`Private/` mirrors `Public/` **without** the module folder: `Private/Logging/Log.cpp`, and private headers are included as `"Logging/Foo.h"`. Only the module itself sees `Private/`, so there is nothing to disambiguate, and a private include never looks like a public one.

## Code style

- Allman braces (the opening brace on its own line) everywhere, including Lua tables. Indent with tabs (enforced by `.editorconfig`). Files are UTF-8 without BOM.

### C++ naming (agreed in M0 step 7)

| Thing | Rule | Example |
|---|---|---|
| Classes and structs | `V` + PascalCase | `VWindow`, `VWindowDesc` |
| Class templates | `T` + PascalCase | `TArray`, `TSet` |
| Template type parameters | `T` + PascalCase | `TElement`, `TArgs` |
| Enums | `enum class`, `E` + PascalCase; values in PascalCase | `ELogLevel::Warning` |
| Functions, members, locals | PascalCase, no `m_` | `RunApplication()`, `LayerStack` |
| Bools (members and locals) | `b` prefix | `bIsRunning` |
| Bool parameters | No `b` | `SetVSync(bool VSync)` |
| Parameters | `In` only when the name would match a member; `Out` always on output parameters | `SetDesc(const VWindowDesc& InDesc)`, `bool& OutSucceeded` |
| Free functions | Inside `namespace Vertex`; internals that a header must expose go inside `Vertex::Private`; helpers used by one `.cpp` only go in an anonymous namespace | `Vertex::InitializeRuntime()` |
| Constants | `constexpr`, PascalCase, never `#define` | `MaxLogLineLength` |
| Macros | `V` prefix, UPPER_SNAKE. `VERTEX_*` and `<MODULE>_API` come from the build. | `VCLASS`, `V_DECLARE_CLASS` |
| Files | Named after the main type, without its prefix | `TArray` → `Array.h` |

Why:
- Types live at global scope, so their prefix is what keeps them from colliding with raylib's and Windows' unprefixed global names (`CloseWindow`, `DrawText`, …). Free functions have no prefix, so they live in `namespace Vertex`.
- Macros are expanded by the preprocessor before namespaces exist, so a prefix is their only protection.
- A method parameter with the same name as a member hides it ("shadowing"), which is warning C4458. At `/W4` with warnings as errors, that fails the build. Hence the `In` rule.

### File layout

Every `.h`, `.cpp`, and `.inl` starts with the copyright line. Headers follow it with `#pragma once`. Includes are written from the module's include root.

```cpp
// Copyright HNDRED GAMES. All Rights Reserved.

#pragma once

#include "Core/Logging/Log.h"
```

A `.cpp` includes its own header first. If that header is missing an include it needs, it fails right there instead of somewhere unrelated, which keeps every header self-sufficient.

### Declaration order (agreed in M0 step 7)

Inside a class or struct, declarations come in three groups, in this order: **types, then functions, then variables**, never interleaved. Each group runs its own `public` → `protected` → `private` sequence, so a group re-states an access specifier even when the previous group ended on the same one. Two adjacent `private:` sections are correct, not redundant. Static data members and callbacks (a `std::function` member, for example) count as variables.

Types come first because C++ only lets a function signature use a type declared above it. A nested type placed among the variables fails to compile (C3646) as soon as a function takes or returns it.

```cpp
class RUNTIME_API VWindow
{
public:
	enum class EMode : std::uint8_t
	{
		Windowed,
		Fullscreen
	};

public:
	explicit VWindow(const VWindowDesc& InDesc);

	EMode GetMode() const;

private:
	void ApplyDesc();

private:
	VWindowDesc Desc;
	EMode Mode = EMode::Windowed;
};
```

A `.cpp` (or `.inl`) defines its functions in the order the header declares them. Helpers in the `.cpp`'s anonymous namespace sit above those definitions.

Why:
- A reader finds the whole API in one block and the whole state in another. The variables block shows what the object owns at a glance.
- Matching order lets you read the `.h` and the `.cpp` side by side. A new function lands in the same place in both files, which keeps diffs predictable.

### Decided when first needed

Concepts, interfaces, global variables, and type aliases. The M2 port has to rename the old `using TSize = size_t;`, which now reads like a class template.

## Git workflow (GitHub Flow)

- **`master` is always green.** It generates and builds in every configuration. Work never lands on it directly; it arrives through pull requests.
- **One short-lived branch per piece of work**, cut from the latest `master`:
  - `feature/<milestone>-<topic>` for new work, e.g. `feature/m0-engine-skeleton`
  - `fix/<topic>`, `chore/<topic>` (build, tooling, cleanup), `docs/<topic>`
- **Commit messages:** an imperative summary of at most 72 characters ("Add Core logging", not "Added…"), a blank line, then *why* in the body.
- **Pull requests into `master`** merge with **squash**, and the branch is deleted. Claude raises and merges PRs as a chore, with the `gh` CLI, after verifying the build. `chore/` and `docs/` PRs hold Claude-owned work and merge once verified. `feature/` and `fix/` PRs hold Fares' engine code: Fares reads the diff first, and Claude merges when he says go.
- **Milestones are tagged** on `master` when complete: `m0`, `m1`, …
- Never force-push `master`. Never commit generated files (`Binaries/`, `Intermediate/`, `Vertex.sln`, `ThirdParty/*/Source/`).

## Unreal Engine reference

UE 5.7.4 source: `C:\Developer Projects\HNDREDGAMES\UE_5_7_4`. Use it as inspiration for every system: find the counterpart and ask *what problem Epic solved, and does Vertex have it yet*. **Study, never copy.** UE code is under Epic's EULA.

### API lookup (unreal-api MCP)

The `unreal-api` MCP indexes UE's gameplay-facing C++ API (`AActor`, `UGameplayStatics`, …): signatures, class members, and `#include` paths. It doesn't index engine internals. Spot checks for `FEngineLoop`, `FGenericPlatformMisc`, and `TArray` found nothing.

| When | Tool | Example |
|---|---|---|
| Seeing how Epic shaped a class's public API | `get_class_reference` | `get_class_reference("UGameplayStatics")` |
| Finding an API by keyword | `search_unreal_api` | `search_unreal_api("spawn actor")` |
| Reading one function's exact signature | `get_function_signature` | `get_function_signature("AActor::GetActorLocation")` |
| Finding a UE type's header, to open it in the local source | `get_include_path` | `get_include_path("ACharacter")` |
| Seeing which designs Epic moved away from, and what replaced them | `get_deprecation_warnings` | `get_deprecation_warnings("K2_AttachRootComponentTo")` |

- It's a study aid, like the source tree. Quote a UE signature to discuss a design; never paste UE code into Vertex.
- For engine internals (Core, HAL, containers, RHI, the engine loop, the header tool), read the local 5.7.4 source.
- Never use it to verify Vertex code. Vertex's APIs and includes come from this repo.
- If the MCP and the local source disagree, the local source wins: 5.7.4 is the version Vertex studies.

## Porting the earlier Vertex

The earlier attempt lives read-only at `C:\Developer Projects\Raylib Projects\Tagbound\Vertex`. Port pieces deliberately, one system at a time, through the rules above. Never bulk-copy. Tagbound pins that repo as a submodule, so never force-push it.
