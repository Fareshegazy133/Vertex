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
- Vertex modules build at `/W4` with warnings as errors; ThirdParty builds with warnings off.
- Generated files go to `Intermediate/ProjectFiles/`, binaries to `Binaries/`. Neither is committed.

## Source layout per module

```
Source/<Module>/
  <Module>.Module.lua
  Public/    headers other modules may include (the include root)
  Private/   .cpp files and internal headers
```

Core's public headers live under `Public/Core/…`, so includes read `#include "Core/Containers/Array.h"`.

## Git workflow (GitHub Flow)

- **`master` is always green.** It generates and builds in every configuration. Work never lands on it directly; it arrives through pull requests.
- **One short-lived branch per piece of work**, cut from the latest `master`:
  - `feature/<milestone>-<topic>` for new work, e.g. `feature/m0-engine-skeleton`
  - `fix/<topic>`, `chore/<topic>` (build, tooling, cleanup), `docs/<topic>`
- **Commit messages:** an imperative summary of at most 72 characters ("Add Core logging", not "Added…"), a blank line, then *why* in the body.
- **Pull requests into `master`:** Fares reads the diff, merges with **squash**, and deletes the branch. Claude prepares the PR title and body; Fares creates and merges it.
- **Milestones are tagged** on `master` when complete: `m0`, `m1`, …
- Never force-push `master`. Never commit generated files (`Binaries/`, `Intermediate/`, `Vertex.sln`, `ThirdParty/*/Source/`).

## Unreal Engine reference

UE 5.7.4 source: `C:\Developer Projects\HNDREDGAMES\UE_5_7_4`. Use it as inspiration for every system: find the counterpart and ask *what problem Epic solved, and does Vertex have it yet*. **Study, never copy.** UE code is under Epic's EULA.

## Porting the earlier Vertex

The earlier attempt lives read-only at `C:\Developer Projects\Raylib Projects\Tagbound\Vertex`. Port pieces deliberately, one system at a time, through the rules above. Never bulk-copy. Tagbound pins that repo as a submodule, so never force-push it.
