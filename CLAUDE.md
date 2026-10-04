# Vertex — Project Contract

Vertex is a C++20 mini engine. raylib is the first backend and must stay replaceable. This file is the contract: if a change breaks a rule here, stop and say which rule and why.

Working mode: Fares is learning engine architecture by building it.
- **Claude writes the build setup:** everything under `Build/`, `Scripts/`, `Vertex.lua`, and every `*.Module.lua`. Also docs and mechanical edits. After writing, walk Fares through what each file does and why.
- **Fares writes all engine C++**, walked through one card at a time (see the user-level CLAUDE.md, Mentorship Mode rule 7).
- **Claude writes every comment** in that C++, as a chore (see Code style § Comments).
- Every feature follows the cycle in **Feature workflow** below.

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
- By-value parameters are `const` in the declaration and the definition alike, so the two signatures stay identical: `void Log(const ELogLevel LogLevel, const std::string_view Message);`. The compiler ignores that `const` in a declaration. In the definition, it stops the body from reassigning a parameter by accident.

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

### Comments

Claude writes and maintains every comment in engine C++ (see Working mode). When code changes, its comments change in the same commit. A stale comment is a bug.

#### What gets a comment

Every declaration in a header, `Public/` or `Private/`, with no exceptions:
- classes, structs, and unions, nested ones included;
- functions, including constructors, destructors, operators, and `= default` or `= delete` members;
- variables at namespace or class scope: members, statics, globals, and constants;
- enums, and every enum value.

Never commented: namespaces, type aliases, macros, concepts, local variables, anything inside a function body, and anything in a `.cpp` or `.inl` besides its copyright line (see Implementation code below).

#### Format

```cpp
/** A comment that fits on one line and has no tags. */

/**
 * A summary of what the declaration is or does.
 *
 * @param Name Tags start after one blank " *" line.
 */
```

- A comment with any tag, or one too long for one line, uses the block form: `/**` alone on its line, then ` * ` lines, then ` */` alone.
- A comment sits on its own line, directly above what it describes.
- Tags follow one another with no blank lines between them. One space separates a tag from its text, with no column alignment: aligned columns get re-padded whenever a longer name arrives, which turns one-line changes into noisy diffs.
- The copyright line, which opens every `.h`, `.cpp`, and `.inl`, is the only `//` comment in the codebase.

#### Content

A comment says **why**: intent, units, valid range, who writes it, the trap a later edit could fall into. It adds what the declaration can't say and never restates the code. `/** The width. */` above `int Width;` adds nothing. `/** Client width in pixels. Only the backend writes it, on resize. */` does.

#### Functions

A short summary of what the function does, then these tags, in this order:

| Tag | When |
|---|---|
| `@tparam` | One per template parameter |
| `@param` | One per parameter |
| `@return` | Every function that returns a value. It says what special values mean (`nullptr`, empty, `-1`). |
| `@warning` | Each thing a caller can get wrong that causes an error: a precondition, an invalidated pointer, the wrong thread |
| `@note` | Each thing a caller should know that won't cause an error: cost, flushing, ordering |

An override gets only a one-line comment saying what differs from the base version. The base's comment holds the contract.

```cpp
/**
 * Writes one line, formatted as "[Level] Message", and adds the newline.
 *
 * @param LogLevel Severity. Info goes to stdout; Warning and Error go to stderr.
 * @param Message The text to write. Read only during the call; it needs no null terminator.
 * @note Safe to call from any thread: each line is written whole, then flushed.
 * @note Every call flushes, which costs a write to the OS. Keep logging out of per-frame code.
 */
CORE_API void Log(const ELogLevel LogLevel, const std::string_view Message);
```

#### Classes, structs, and unions

A summary of what the type is and the role it plays in the engine, then these tags, in this order:

| Tag | The question it answers | Required |
|---|---|---|
| `@inherits <Base>` | What does this type change from that base, and why does it derive from it? One per direct base. | Every derived type |
| `@ownership` | Who creates, owns, and destroys it? Can it be copied or moved? Does it own its resources or borrow them? | Always |
| `@lifetime` | When does it become valid, when does it die, and what invalidates it or pointers into it? | Always |
| `@threading` | Which threads may use it, and what's guaranteed? | Always |
| `@networking` | Who has authority? Is it local only, server-owned, or replicated, and to whom? | Always |
| `@performance` | Does it allocate? What do its key operations cost? Is it safe on a per-frame path? | When it matters |
| `@backend` | Which backend does it hide or depend on? | Runtime types that touch the platform |
| `@invariants` | What's always true about a valid instance? | When it has any |

```cpp
/**
 * The application's OS window. It owns the native window and presents each rendered frame.
 *
 * @ownership Owned by the engine loop. Non-copyable: it owns the native window handle.
 * @lifetime Valid from construction to destruction.
 * @threading Game thread only.
 * @networking Local only. A dedicated server never creates one.
 * @backend Backend-neutral. The raylib translation lives in Platform/Raylib.
 */
class RUNTIME_API VWindow
```

#### Enums

The enum's comment says what it's used for. Each value's comment says what that value represents. One blank line follows every value except the last, so each comment visibly belongs to the value below it, not the one above.

```cpp
/** How severe a log line is. It picks the line's prefix and the stream it goes to. */
enum class ELogLevel : std::uint8_t
{
	/** Normal operation worth recording: startup, shutdown, state changes. */
	Info,

	/** Something unexpected happened, and the engine recovered. */
	Warning,

	/** An operation failed, and its result is missing or wrong. */
	Error
};
```

An enum that is saved to disk or sent over the network also carries `@warning Values are serialized: append new ones at the end; never reorder or remove.` Reordering one silently breaks old save files, and clients running another version.

#### Header comments

A header gets a file comment only when it holds two or more top-level classes, structs, or unions, or none at all (only functions, variables, enums, and so on). With exactly one top-level type, that type's comment already describes the file.

The file comment sits between the copyright line and `#pragma once`. It says what the file provides as a unit and why these declarations share a file. With several types, it says which one to read first. `@see` pointing at related headers is optional. It never lists the file's contents: that repeats the code and goes stale with the next declaration.

```cpp
// Copyright HNDRED GAMES. All Rights Reserved.

/** Core logging: the severity levels and the function that writes one line to the console. */

#pragma once
```

#### Implementation code

`.cpp` and `.inl` files carry no comments besides the copyright line, and no function body carries one, wherever it lives. The knowledge an implementation line depends on goes somewhere sturdier:
- **If a caller relies on it, the header states it as a contract** (`@note`, `@warning`). `Log`'s notes promise whole lines and a flush on every call. Deleting the `fflush` would break that documented promise, which is what stops a later edit.
- **If the compiler enforces it, nothing is needed.** A return after an exhaustive `switch` (C4715) can't be deleted without failing the build.
- **Otherwise, the commit message says why.** `git log -L` and `git blame` find it next to the line's history.

Block comments don't nest. A `/* … */` wrapped around code that contains a doc comment ends at that comment's `*/`. Disable code with `#if 0` … `#endif` instead.

Why:
- Doxygen and IDE hovers only treat `/**` as documentation, and UE uses the same form for one-line and block comments. A single marker also keeps a future HeaderTool simple if it turns comments into editor tooltips, the way UHT does.
- The function tags are Doxygen's standard set (`@note`, not `@info`), so tools understand them. The class tags are Vertex's own, because Doxygen has none for these questions.
- The four always-required class tags make an omission visible. A missing line could mean "forgot" or "doesn't apply", while `@networking Local only.` records a decision, made on the day the type is written.

### Decided when first needed

Concepts, interfaces, global variables, and type aliases. The M2 port has to rename the old `using TSize = size_t;`, which now reads like a class template.

## Feature workflow

Every feature runs the same cycle, in one conversation. Fares clears the chat between features, so anything the next conversation needs is saved before the cycle ends.

1. **Plan together.** Discuss the problem, the design forks, and the UE counterpart. Claude recommends; Fares makes the calls.
2. **Write the plan.** Claude cuts a `feature/<milestone>-<topic>` branch and writes the plan to `.claude/plans/<milestone>-<topic>.md`. The plan covers the decisions and why, the files to create or change, the walkthrough cards in dependency order, and how to verify. Fares reviews it. Nothing is implemented until he approves.
3. **Implement, one card at a time.** Claude walks Fares through each card (user-level CLAUDE.md, Mentorship Mode rule 7). Fares writes the engine C++. Claude does the chores.
4. **Review.** When Fares says the feature is done, Claude runs `/review`. Fares fixes every ERROR and WARNING, and Claude reviews again until it says **"Review passed."**
5. **PR and merge.** Claude commits, raises the PR, and merges it once Fares has read the diff and says go (see Git workflow).
6. **Hand off.** Claude says what's next, marks the plan done, saves what the next conversation needs to its memory, and tells Fares it's safe to clear the chat.

The plan is committed on the feature branch, so the PR carries the design and a later conversation can read it.

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
