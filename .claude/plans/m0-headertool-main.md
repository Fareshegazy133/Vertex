# M0 card 2: HeaderTool `main()`

**Status:** done 2026-10-08. Both cards implemented; `/review --build` passed.

Branch `feature/m0-headertool-main`. Repo copy of this plan: `.claude/plans/m0-headertool-main.md`.

## Context

Core logging is merged (PR #7). `VertexCore.lib` builds, but a static library never resolves symbols. `Log`'s definition has not been checked by a real link yet. HeaderTool is the smallest executable that can do that check: it depends on Core only (`Source/Programs/HeaderTool/HeaderTool.Module.lua`).

The card also fixes HeaderTool's contract with build systems early. In M5, MSBuild runs HeaderTool as a pre-build step and acts on two things only: the exit code and stderr. Nothing generates code yet, so M0 ships a stub of that contract. It's honest about doing nothing, and it's correct about how it fails.

Outcome: HeaderTool has its own part of the full-solution link. The full solution still won't link until cards 3–4 give Runtime and Editor a `.cpp`.

## Decisions (Fares, 2026-10-04)

| Fork | Decision | Why |
|---|---|---|
| Behavior | **Stub the M5 contract.** Exactly one argument (a manifest path, not opened until M5) → Info "nothing to generate yet", exit 0. Anything else → **one** Error line that carries the usage, exit 1. | It exercises stdout, stderr, and the exit code. Usage printed because of an error belongs on stderr, so `2>$null` hides all of it and no orphaned usage line is left behind. The manifest-path shape follows UHT: command lines are capped (32,767 characters for `CreateProcess`, 8,191 for `cmd.exe`), and hundreds of header paths won't fit. The manifest *format* is decided in M5. |
| `main` shape | **Thin `main` → `Run`.** `main` only adapts `ArgC`/`ArgV` into a `std::span<const char* const>`, skipping the program name safely, and returns `Run(Arguments)`. `Run` is a helper in the `.cpp`'s anonymous namespace. | `main` is the OS boundary: a raw pointer, a separate count, and an encoding the OS picks. Everything below it gets a span that carries its own bounds and owns nothing. UE keeps `WinMain` thin the same way (it hands off to `GuardedMain`). Later, UTF-8 conversion, a crash guard, or a test that calls `Run` with fake arguments each have one place to go. |
| UTF-8 `argv` | **Deferred, with a trigger** (see Deferred). | M0 passes no real paths. |

Settled defaults:
- **`argc` can be 0.** The standard allows it, and CVE-2021-4034 (PwnKit) came from a program that read `argv[1]` without checking. With no program name, `main` passes an empty span: calling `subspan(1)` on an empty span is undefined behavior.
- **Exit codes** are `EXIT_SUCCESS` and `EXIT_FAILURE` from `<cstdlib>`. A named `E…` enum arrives in M5 with a third outcome, as UE's `ECompilationResult::UpToDate` shows.
- **Messages are fixed text.** Printing the manifest path would need formatting, which is the Logging v2 trigger and outside this card's scope.

## UE references (study, never copy)

Paths are relative to `C:\Developer Projects\HNDREDGAMES\UE_5_7_4\Engine\Source\`.
- `Programs/BlankProgram/Private/BlankProgram.cpp`: UE's minimal program that uses only Core.
- `Runtime/Core/Public/HAL/Platform.h:1021`: `INT32_MAIN_INT32_ARGC_TCHAR_ARGV()` expands to `wmain` on Windows. That's UE's answer to the UTF-8 `argv` question.
- `Runtime/Launch/Private/Windows/LaunchWindows.cpp:113`: `GuardedMainWrapper`. The OS entry point stays thin, and the crash guard lives in one place.
- `Programs/UnrealBuildTool/Modes/UnrealHeaderToolMode.cs:1085` (usage line) and `:1233` (`WriteUHTManifest`): the build tool writes the manifest, and UHT receives its path.

## Files

| File | Change | Owner |
|---|---|---|
| `Source/Programs/HeaderTool/Private/HeaderToolMain.cpp` | New. Create it on disk, not through the IDE. | Fares |
| `.claude/plans/m0-headertool-main.md` | This plan, committed on the branch | Claude |

`HeaderTool.Module.lua` needs no change. The `.cpp` has no header, and its only comment is the copyright line (CLAUDE.md § Implementation code). The *why* behind the `argc == 0` guard goes in the commit message.

## Cards (one at a time; Fares writes the C++; no check-in questions since 2026-10-05)

### Card 1: the first link (done)
- **What:** `HeaderToolMain.cpp` with an `int main()` that has no parameters and returns `Run()`. `Run()` logs `[Info] Nothing to generate yet: reflection arrives in M5` and returns `EXIT_SUCCESS`.
- **Why:** this is the smallest slice that makes the linker resolve `Vertex::Log` from `VertexCore.lib`. Parameters wait for card 2, because an unused `ArgC`/`ArgV` would be C4100, an error at `/W4` with warnings as errors.
- **Includes:** `"Core/Logging/Log.h"`, `<cstdlib>`.
- **Verify:**
  1. `Scripts\Setup.bat` (a new file needs regeneration).
  2. Build `Intermediate\ProjectFiles\HeaderTool.vcxproj` with MSBuild in Debug, Development, and Shipping. Expect `0 Warning(s)` and `0 Error(s)`. The project reference builds Core first.
  3. `.\Binaries\Win64-Debug\VertexHeaderTool.exe; $LASTEXITCODE` should print the Info line, then `0`.
  4. `dumpbin /dependents` on the exe (MSVC 14.44 `bin\Hostx64\x64\dumpbin.exe`) should list only the CRT and KERNEL32: no `opengl32`, `winmm`, or `gdi32`.

### Card 2: the boundary and the contract (done)
- **What:** `main(const int ArgC, char** const ArgV)` builds the argument span without the program name and returns `Run(Arguments)`. `Run(const std::span<const char* const> Arguments)` applies the contract: `size() != 1` → `[Error] Expected one argument. Usage: VertexHeaderTool <ManifestPath>`, then `EXIT_FAILURE`. Otherwise it logs the Info line and returns `EXIT_SUCCESS`.
- **Traps to teach on the card:**
  - Only a *top-level* `const` is allowed on `main`'s parameters. `char* const* ArgV` would change `main`'s type, which makes it non-standard.
  - The `ArgC == 0` guard, and the reason `subspan(1)` needs it.
  - The `char**` → `std::span<const char* const>` conversion. Compile-check it on MSVC 14.44 before presenting the card.
- **Includes:** add `<span>` and `<cstddef>`.
- **Verify:** all three configurations build with zero warnings, then this matrix (PowerShell, `Binaries\Win64-Debug\`):

| Command | Prints | `$LASTEXITCODE` |
|---|---|---|
| `VertexHeaderTool.exe` | `[Error] Expected one argument. Usage: …` | 1 |
| `VertexHeaderTool.exe 2>$null` | nothing | 1 |
| `VertexHeaderTool.exe Fake.json` | `[Info] Nothing to generate yet: …` | 0 |
| `VertexHeaderTool.exe Fake.json >$null` | nothing | 0 |
| `VertexHeaderTool.exe A B` | the Error line | 1 |


## After the cards (feature workflow steps 4–6)

1. **Review:** Claude runs `/review`. Fares fixes every ERROR and WARNING until it says "Review passed."
2. **PR:** Claude commits (summary "Add HeaderTool entry point"; the body explains the `argc == 0` guard and why the usage goes to stderr) and raises the PR. Fares reads the diff, and Claude merges on his go (squash; `--subject "<Title> (#N)"`).
3. **Hand off:** Claude marks this plan done, updates memory (card 2 done, next is card 3, the Runtime probe), and tells Fares it's safe to clear the chat.

## Deferred

- **UTF-8 `argv` on Windows.** Windows passes narrow `argv` in the ANSI code page, so a path like `C:\Users\Zoë\…` arrives mangled. **Trigger:** M1, when the Editor parses `-backend=` and Core gets its first command-line facility, and in any case before M5 opens a manifest path. **Options then:** a UTF-8 `activeCodePage` app manifest (a build-layer chore), or `wmain` plus a conversion (UE's route).
- **Exit-code enum:** M5, with its first third outcome.
- **MSBuild-clickable diagnostics** (`file(line,col): error VHT0001: …`): M5, when HeaderTool reports parse errors.
