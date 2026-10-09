# M0 cards 3–4: the Runtime probe and the Editor entry point

**Status:** done 2026-10-08. All four cards are implemented and `/review --build` passed. After the review, the backend file was renamed `RaylibPlatformInit.cpp`, under the new "Backend `.cpp` files" naming row (Fares chose option A).

Branch `feature/m0-runtime-probe`. Repo copy of this plan: `.claude/plans/m0-runtime-probe.md`.

## Context

HeaderTool proved that Core links into an executable (PR #11). Nothing has proved the other half of the module graph yet. Can the Editor reach raylib through Runtime without ever naming raylib? And does Runtime's internal layering hold once code exists?

`master` doesn't build. Editor fails with `LNK1104: cannot open file 'VertexRuntime.lib'`: Runtime has no source, so the library is never produced. Card 3 alone would trade that for LNK1561, because Editor has no `main`. So cards 3 and 4 ship as one feature, and the full solution builds in every configuration when it lands.

Outcome: `VertexEditor.exe` runs, prints one line from the raylib backend and one from the Editor, and exits 0. Every include on the call path points down:

```
EditorMain.cpp ──→ Engine/Engine.h ──→ Platform/PlatformInit.h ──→ (RaylibPlatformInit.cpp) ──→ raylib
   Editor            Runtime public       Runtime private               Runtime private       ThirdParty
```

## Decisions (Fares, 2026-10-08)

| Fork | Decision | Why |
|---|---|---|
| Scope | **Cards 3 and 4 in one feature.** | A static library never resolves symbols, so the raylib call isn't proven until an executable links it. Shipping card 3 alone would leave `master` red. |
| Layering | **Engine drives Platform.** `Public/Engine/Engine.h` declares `Vertex::InitializeEngine()`, and `Private/Engine/Engine.cpp` defines it. It calls `Vertex::InitializePlatform()`, which is declared in the Runtime-private `Private/Platform/PlatformInit.h` and defined in `Private/Platform/Raylib/RaylibPlatformInit.cpp`. | The original idea declared the function in `Engine/` and defined it in `Platform/Raylib/`. The definition includes its declaration, so Platform would include Engine: an upward edge, and a dependency cycle once Platform becomes a module. Engine-drives-Platform is UE's shape (see UE references). M1's `VEngineLoop` grows from it without the Editor's include changing. |
| raylib call | **`SetTraceLogLevel(LOG_WARNING)`.** | Any raylib function pulls in `rcore.obj`: raylib 6.0's `rcore.c` `#include`s the GLFW platform file and rlgl's implementation. So any call proves the whole chain. This one has no side effects and nothing to shut down, and it keeps raylib's own `printf` INFO lines from going around `Vertex::Log`. |

Settled defaults:
- **Runtime's public include root has no `Runtime/` prefix** (CLAUDE.md § Source layout, updated on this branch). Each layer folder is a module-in-waiting, so `"Engine/Engine.h"` survives a promotion unchanged.
- **A Runtime private header never shares its path with a public one.** Both folders are on Runtime's include path, so the first match wins without a warning. That's why the private header is `PlatformInit.h` and not `Platform.h`: M1 may add a public `Platform/` header with that name.
- **The Engine function is `InitializeEngine()`** (Fares, card 2; this plan first said `InitializeRuntime()`). Each layer's startup function is named after its layer, like `InitializePlatform()`.
- **`InitializePlatform` has no `RUNTIME_API`.** It never leaves Runtime. `InitializeEngine` crosses into the Editor, so it carries `RUNTIME_API`.
- **Both functions live in plain `namespace Vertex`.** `Vertex::Private` is for internals that a *public* header must expose. `PlatformInit.h` is already private by its location.
- **No `ShutdownRuntime()`.** Nothing needs releasing yet. M1's `VEngineLoop` brings the full lifecycle (`PreInit`/`Init`/`Tick`/`Exit`), and an empty shutdown today would be a contract nothing tests.
- **`main()` takes no parameters.** Unused `ArgC`/`ArgV` would be C4100, an error at `/W4`. M1 adds them for `-backend=`, with HeaderTool's thin `main` → `Run` shape.
- **The log text needs no formatting.** `"Platform initialized: raylib " RAYLIB_VERSION` joins two string literals at compile time, so it doesn't trigger Logging v2.

## UE references (study, never copy)

Paths are relative to `C:\Developer Projects\HNDREDGAMES\UE_5_7_4\Engine\Source\`.
- `Runtime/Launch/Private/LaunchEngineLoop.cpp:1699`: `FEngineLoop::PreInitPreStartupScreen`. At `:2833` it calls `FPlatformMisc::PlatformInit()`. The engine layer drives the platform; the platform never calls up.
- `Runtime/Core/Public/GenericPlatform/GenericPlatformMisc.h:566`: `static void PlatformInit() { }`, the backend-neutral declaration with a no-op default.
- `Runtime/Core/Private/Windows/WindowsPlatformMisc.cpp:946`: `FWindowsPlatformMisc::PlatformInit()`, the Windows definition.
- `Runtime/Core/Public/HAL/PlatformMisc.h:6`: `COMPILED_PLATFORM_HEADER` picks the platform at compile time. Vertex picks at link time in M0 (only the raylib `.cpp` exists), then at run time in M1 (`-backend=raylib|null`).

## Files

| File | Change | Owner |
|---|---|---|
| `Source/Runtime/Private/Platform/PlatformInit.h` | New: declares `Vertex::InitializePlatform()` | Fares |
| `Source/Runtime/Private/Platform/Raylib/RaylibPlatformInit.cpp` | New: the raylib definition | Fares |
| `Source/Runtime/Public/Engine/Engine.h` | New: declares `RUNTIME_API void Vertex::InitializeEngine()` | Fares |
| `Source/Runtime/Private/Engine/Engine.cpp` | New: defines it; calls `InitializePlatform()` | Fares |
| `Source/Editor/Private/EditorMain.cpp` | New: `int main()` | Fares |
| Doc comments in both headers | After each card (CLAUDE.md § Comments) | Claude |
| `CLAUDE.md` § Source layout | Runtime include root, and the private/public path rule | Claude (done on this branch) |
| `.claude/plans/m0-runtime-probe.md` | This plan | Claude |

Create every file on disk under `Source/`, never through the IDE. An IDE "Add New File" lands in `Intermediate/ProjectFiles/`. Run `Scripts\Setup.bat` after creating files: new files are only picked up at generation time.

No descriptor changes: `Runtime.Module.lua` already depends on raylib privately, and `Modules.lua` attaches raylib's `SystemLibraries` only to executables.

## Cards (one at a time; Fares writes the C++; no check-in questions)

### Card 1: the Platform layer
- **What:** `PlatformInit.h` declares `void InitializePlatform();` in `namespace Vertex`. `RaylibPlatformInit.cpp` includes its own header first, then `"Core/Logging/Log.h"`, then `"raylib.h"`, and defines `void Vertex::InitializePlatform()` (qualified, as `Log.cpp` does). The body calls `SetTraceLogLevel(LOG_WARNING);`, then `Log(ELogLevel::Info, "Platform initialized: raylib " RAYLIB_VERSION);`.
- **Why:** the declaration knows nothing about raylib, so Engine can include it. Only the definition is backend-specific, and it sits in the one folder the raylib rule allows.
- **Verify:** `Scripts\Setup.bat`, then build `Intermediate\ProjectFiles\Runtime.vcxproj` in Debug: `0 Warning(s)`, `0 Error(s)`, and `Intermediate\Build\Debug\Runtime\VertexRuntime.lib` exists. `SetTraceLogLevel` is still an unresolved symbol inside that `.lib`. Nothing resolves it until card 3.

### Card 2: the Engine entry point
- **What:** `Engine.h` declares `RUNTIME_API void InitializeEngine();` in `namespace Vertex`. `Engine.cpp` includes `"Engine/Engine.h"`, then `"Platform/PlatformInit.h"`, and defines `void Vertex::InitializeEngine()`, which calls `InitializePlatform();`.
- **Why:** this is the Editor's only door into Runtime. Engine is the top layer, so including Platform points down.
- **Verify:** `Scripts\Setup.bat`, then `Runtime.vcxproj` in Debug, Development, and Shipping: zero warnings in each.

### Card 3: the Editor entry point
- **What:** `EditorMain.cpp` includes `"Core/Logging/Log.h"`, `"Engine/Engine.h"`, and `<cstdlib>`. `int main()` calls `Vertex::InitializeEngine();`, logs `Vertex::Log(ELogLevel::Info, "Editor running. Window arrives in M1");`, and returns `EXIT_SUCCESS`.
- **Why:** this is the first link of Runtime into an executable. The linker must now resolve `SetTraceLogLevel` from `raylib.lib`, plus every Windows call GLFW makes, from the system libraries `Modules.lua` gave the Editor. The Editor includes `Log.h` itself (include what you use). It can see it because Runtime depends on Core publicly.
- **Verify:**
  1. `Scripts\Setup.bat`.
  2. The full `Vertex.sln` in Debug, Development, and Shipping: `0 Warning(s)`, `0 Error(s)`. This is the first green `master` since Runtime was declared.
  3. `.\Binaries\Win64-Debug\VertexEditor.exe; $LASTEXITCODE` prints:
     ```
     [Info] Platform initialized: raylib 6.0
     [Info] Editor running. Window arrives in M1
     0
     ```
  4. `dumpbin /dependents` (MSVC 14.44 `bin\Hostx64\x64\dumpbin.exe`):

     | Exe | Debug | Development and Shipping |
     |---|---|---|
     | `VertexEditor.exe` | CRT, `GDI32`, `WINMM`, `SHELL32`, `KERNEL32`, `USER32` | the same, without `WINMM` |
     | `VertexHeaderTool.exe` | CRT and `KERNEL32` only | CRT and `KERNEL32` only |

     Two things to explain on the card:
     - **`OPENGL32` never appears**, although `opengl32.lib` is linked. GLFW loads `opengl32.dll` at run time (`_glfwPlatformLoadModule`, `external/glfw/src/wgl_context.c:415`), so the exe has no static import of it.
     - **`WINMM` appears in Debug only.** Development and Shipping link with `/OPT:REF`, which drops functions nobody calls. `timeBeginPeriod`/`timeEndPeriod` are reached only from `InitWindow`/`CloseWindow`, so their import goes with them. So `dumpbin` shows what the exe *calls*, not what it *links*.

### Card 4: prove the boundaries (prove, then revert; nothing here lands)
1. **Stream ordering, as promised in PR #7.** Temporarily add `Vertex::Log(ELogLevel::Warning, "Ordering test: stderr");` between `InitializeEngine()` and the Editor's Info line. Build the Editor, then run `cmd /c "Binaries\Win64-Debug\VertexEditor.exe > Order.txt 2>&1"` and `Get-Content Order.txt`. Expect Info, Warning, Info, in call order. Use `cmd`, because Windows PowerShell 5.1 wraps a native program's stderr lines in error records. Revert, and delete `Order.txt`.
2. **raylib stays private.** Add `#include "raylib.h"` to `EditorMain.cpp` and build the Editor. Expect `error C1083: Cannot open include file: 'raylib.h'`. Revert.
3. **HeaderTool can't see Runtime.** Add `#include "Engine/Engine.h"` to `HeaderToolMain.cpp` and build HeaderTool. Expect `error C1083: Cannot open include file: 'Engine/Engine.h'`. Revert.
- **Verify:** `git status` shows only this feature's files. Nothing from card 4 is left behind.

Everything above was prototyped on MSVC 14.44 in a scratch copy of the repo before this plan was written. All three configurations built with zero compiler and linker warnings, and every expected output above was observed.

## After the cards (feature workflow steps 4–6)

1. **Review:** on Fares' word, Claude runs `/review`. Fares fixes every ERROR and WARNING until it says "Review passed." Then Claude waits for Fares' go.
2. **PR:** on Fares' go, Claude commits (summary "Add the Runtime probe and the Editor entry point") and raises the PR. Fares reads the diff, and Claude merges on his go (squash; `--subject "<Title> (#N)"`).
3. **Hand off:** Claude marks this plan done and updates memory. The rest of the M0 verification is build-layer work, and Claude runs it: a fresh clone, the second Setup run, the tamper test, and the cycle error. Then `m0` is tagged.

## Deferred

- **Routing raylib's log through `Vertex::Log`** (`SetTraceLogCallback`, a `va_list` formatter, a severity map). **Trigger:** M1's first window, which is when raylib starts printing. It pairs with Logging v2.
- **Choosing the backend at run time** (a `VWindow` factory, `-backend=raylib|null`): M1.
- **The engine lifecycle** (`VEngineLoop`: `PreInit`/`Init`/`Tick`/`Exit`): M1 absorbs `InitializeEngine()`.
- **A StyleScan check for the private/public path rule.** Offered 2026-10-08. It's mechanical: a Runtime `Private/X/Y.h` with the same path as `Public/X/Y.h` is an ERROR.
