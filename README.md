# Vertex

A small C++20 game engine. raylib is the first backend, but it's hidden behind engine-owned interfaces so it can be replaced later (OpenGL, Vulkan, …) without touching engine or editor code.

> **Status:** M0. The build layer and setup scripts work; the first engine C++ is next.

## Prerequisites

- Windows 10/11, x64
- Visual Studio 2022 with the **Desktop development with C++** workload
- Git (the setup step fetches third-party sources with it)

## Getting started

```bat
Scripts\Setup.bat             :: fetches pinned third-party sources, then generates Vertex.sln
Scripts\Setup.bat --refetch   :: after bumping a third-party version: moves existing clones to the new pin
Scripts\Clean.bat             :: deletes Binaries\, Intermediate\ and Vertex.sln (keeps fetched sources)
```

Open `Vertex.sln` and build. `VertexEditor` is the startup project.

## Modules

```
raylib (ThirdParty) ←private── Runtime ──public──→ Core
                                  ↑                  ↑
                               Editor           HeaderTool
```

| Module | Kind | Purpose |
|---|---|---|
| `Core` | Static library | Types, containers, memory, strings, math, logging. No platform or rendering code. |
| `Runtime` | Static library | Platform, input, rendering, scene. The only module that knows raylib exists. |
| `Editor` | Application | The editor executable. |
| `HeaderTool` | Application | Build-time code generator for reflection. Depends on Core only. |
| `raylib` | Static library (C) | Third-party, fetched at a pinned commit. |

## Repository layout

| Path | Contents |
|---|---|
| `Vertex.lua` | Workspace descriptor (pure data) |
| `Source/<Module>/<Module>.Module.lua` | Module descriptors (pure data) |
| `Build/Premake/` | Everything premake-specific: turns descriptors into a Visual Studio solution |
| `ThirdParty/<Name>/` | Third-party descriptors; fetched sources land in `Source/` (not committed) |
| `Scripts/` | `Setup.bat`, `Clean.bat` |
| `Binaries/`, `Intermediate/` | Build output (not committed) |

See `CLAUDE.md` for the architecture rules this layout enforces.
