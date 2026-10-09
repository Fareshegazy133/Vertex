---
name: review
description: Two-pass review of Vertex changes. Pass 1 checks standards and code style against the project CLAUDE.md (module graph, raylib rule, naming, file layout, formatting, /W4 traps). Pass 2 is a principal engine-engineer review (architecture, ownership, performance, build configurations). Use when Fares asks to review his code, a diff, a branch, or files before committing.
argument-hint: "[branch [base] | all | <path>...] [--build]"
allowed-tools: Read Grep Glob Bash(git status *) Bash(git diff *) Bash(git -c core.safecrlf=false diff *) Bash(git ls-files *) Bash(git log *) Bash(git merge-base *) Bash(git rev-parse *) Bash(bash "${CLAUDE_SKILL_DIR}/Scripts/StyleScan.sh" *)
---

# Vertex Code Review

Two passes over Fares' changes:

1. **Standards & Style.** Does the code follow the contract in CLAUDE.md, and is its style consistent?
2. **Principal Engineer.** Is it the right design for an engine that has to scale?

## Who you're reviewing for

Fares is learning engine architecture by building Vertex. Review like a kind senior colleague (user CLAUDE.md, Beginner Calibration rule 6):

- Start with what's good, specifically and why.
- Then what to change, ordered correctness → design → style, each with the principle behind it.
- He knows C++. Spend the explanation on the build and link model, MSVC warning traps, performance, API design, and engine architecture, not language basics.
- Match depth to the system: a few lines for utilities (logging, small helpers), full depth for architecture (module boundaries, the platform seam, the RHI, reflection, the engine loop).
- **Report, don't fix.** Engine C++ is his to change. For a non-trivial fix, offer a walkthrough card. Build files (`Build/`, `Scripts/`, `Vertex.lua`, `*.Module.lua`), docs, and this skill belong to Claude, so offer to fix those directly.
- **Comments in engine C++ are Claude's chore too** (CLAUDE.md § Code style, Comments). Report a missing or stale comment as `Owner: Claude`, and add it after delivering the review. Never ask Fares to write one.

## Governing rules

- The project **CLAUDE.md** is the contract. Enforce every rule in it, including rules added after this skill was written. If this checklist and CLAUDE.md disagree, CLAUDE.md wins. Say that the skill is stale.
- Cite rules by section, for example "CLAUDE.md § The raylib rule, 2". Don't restate them.
- Don't import rules from other projects that CLAUDE.md doesn't have. If the code is inconsistent where the contract is silent, raise it under **Convention governance** below.

## Scope

Arguments: `$ARGUMENTS`

| Argument | What to review | Style scan |
|---|---|---|
| *(none)* | The working tree: tracked changes vs `HEAD`, **plus untracked files** (new files never appear in `git diff`). The context below is already this scope. | Injected below |
| `branch [base]` | Everything on this branch since it left `base` (default `master`), committed or not. Run `git merge-base <base> HEAD`, then `git -c core.safecrlf=false diff <sha>`, and list untracked files. | `--branch <base>` |
| `all` | The whole codebase: `Source/`, the build descriptors, `Build/Premake/`, `Scripts/`. Read every file. | `--all` |
| `<path>...` | Only those files or folders, read in full. | `<path>...` |
| `--build` | Combines with any of the above. See **Build check**. | |

Run the scan for a non-default scope with `bash "${CLAUDE_SKILL_DIR}/Scripts/StyleScan.sh" <mode>`.

- If there are no arguments and the working tree is clean, don't proceed silently. Ask Fares whether to review the branch vs `master` or the whole codebase.
- Skip `.claude/`, `.idea/`, and generated output (`Binaries/`, `Intermediate/`, `Vertex.sln`, `ThirdParty/*/Source/`) unless asked.
- Read every untracked file in full. For modified files, read enough surrounding code to judge the change. Don't review code you haven't seen.
- State the scope in the report header.

## Context (working tree)

Branch and status:
!`git status --short --branch`

Untracked files (not in the diff below; read each one in full):
!`git ls-files --others --exclude-standard -- . ":(exclude).claude"`

Diff vs `HEAD` (tracked files):
!`git -c core.safecrlf=false diff HEAD`

Style scan (changed and untracked files):
!`bash "${CLAUDE_SKILL_DIR}/Scripts/StyleScan.sh" --changed`

## Build check (`--build` only)

A review can't claim the code compiles without compiling it. With `--build`:

1. If the change adds, removes, or renames source files, run `Scripts\Setup.bat` first. New files only reach the projects at generation time.
2. Build each touched module's project in **Debug** and **Shipping** (PowerShell, from the repo root):
   `& "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" Intermediate\ProjectFiles\<Module>.vcxproj -m -nologo -v:minimal -p:Configuration=<Config> -p:Platform=x64`
   Debug and Shipping sit on opposite sides of `VERTEX_ENABLE_ASSERTS`, so together they catch code that only breaks in one of them.
3. Every compiler error is an ERROR finding at its `file:line`. Quote the first error, not the last; later errors are often fallout.
4. If the change touches Core or its tests, also build `CoreTests` in both configurations and run `Binaries\Win64-<Config>\VertexCoreTests.exe`. Every failed test is an ERROR at the `file(line)` doctest prints.
5. If a project fails for a reason unrelated to the change (for example, a module with no `.cpp` yet produces no `.lib`), say so. Don't count it as a finding.

Without `--build`, the report header says "Not built", and the review names the configurations the change is at risk in.

---

# Pass 1: Standards & Style

Check every changed line, and every line of new files. Report only what you can see.

**Start from the style scan.** Its hits are candidates. Confirm each against the source, drop false positives, and report how many you dropped. The scan covers the mechanical rules a diff hides (tabs vs spaces, BOMs, trailing whitespace, brace placement) plus the regex-checkable naming and include rules. Everything below that it can't see is yours to check by reading.

### 1. Module boundaries (CLAUDE.md § Module graph)
- Core contains no platform, windowing, rendering, or generated code.
- Runtime depends on raylib **privately**. Editor depends on Runtime only. HeaderTool depends on Core only. CoreTests depends on Core and doctest only. Runtime never depends on Editor, and no module that ships depends on doctest.
- A change to `PublicDependencies` or `PrivateDependencies` matches the graph. A public dependency hands its headers to every consumer, so making one public needs a reason.
- An include of another module's header is backed by a declared dependency, not by a transitive accident.

### 2. The raylib rule (CLAUDE.md § The raylib rule)
- raylib headers appear only under `Source/Runtime/Private/Platform/Raylib/`.
- No raylib type, enum, or macro in any `Public/` header. Vertex owns the vocabulary (`VKey`, `VColor`, …) and the backend translates.
- `raylib.h` and `<windows.h>` never meet in one translation unit. Check this **transitively**: a backend `.cpp` that includes a Vertex header that includes `<windows.h>` counts. The scan only sees direct includes.

### 3. Runtime layering (CLAUDE.md § Runtime layering)
- Platform → Core. Input and Render → Core, Platform. Scene → Core, Input, Render. Engine → everything above.
- An include that points up the list is an ERROR.
- A proposal to promote a folder to a module cites one of the four triggers.

### 4. Naming (CLAUDE.md § C++ naming)
Check every row of the table. The scan covers type, enum, template-parameter, macro, `m_`, and simple `bool` names. You check the rest:
- Functions, members, and locals are PascalCase. Flag `camelCase` and `snake_case` in Vertex identifiers.
- `In` appears only where a parameter would shadow a member. `Out` appears on **every** output parameter (a non-const reference or pointer the function writes).
- Free functions live in `namespace Vertex`. Internals a header must expose go in `Vertex::Private`. Helpers used by one `.cpp` go in an anonymous namespace, not `static`.
- Constants are `constexpr` PascalCase, never `#define`.
- A file is named after its main type, without the prefix (`TArray` → `Array.h`).
- A class template gets `T`, a plain class gets `V`. The scan can't tell which one a class is.
- Getters (the Getters row): `Get…` always succeeds, so a `Get` that can return `nullptr`, an empty `std::optional`, or a not-found index is really a `Find`. A `bool` getter never starts with `Get` or `Find`. No `TryGet…`.

### 5. File layout (CLAUDE.md § File layout, § Declaration order, § Source layout per module)
- The copyright line comes first in every `.h`, `.cpp`, and `.inl`. Headers follow it with `#pragma once`.
- **Class layout (CLAUDE.md § Declaration order).** The scan can't see this yet, so read every changed class, struct, and union, and check each rule:
  - Types, then functions, then variables. Each group runs `public` → `protected` → `private`. Flag a data member inside the functions block, and a function declared below the first variable. Two adjacent sections with the same access specifier are correct.
  - Types group: macros first, above the first access specifier, then aliases and nested types, each declared before anything that uses it.
  - Functions, in each access section, follow the 13 rows of the table. Classify each function: constructors, the destructor, and operators go to their own rows by syntax. Every other function goes to the first of rows 4–12 it fits. Then confirm the rows never go backwards. Quote the out-of-place function and the row it belongs in.
  - Row 2: your own parameter lists, then copy, then move. Row 4: new virtuals, then overrides grouped by base class in declaration order.
  - Getter vs normal: a getter is `const`, has no output parameters, and is named as a question or a noun. A function that changes the object, or is named for an action, is normal, whatever it returns.
  - Operators follow their seven-step order: `=`; compound assignment; their binary partners in matching order; comparison; `[]` then `()`; others; conversions.
  - A `const`/non-`const` overload pair stays adjacent, `const` version first. Related functions stay together within a row.
  - Variable order within an access section is undecided (§ Decided when first needed). Don't flag it. If a pattern shows up, raise it under **Convention governance**.
- A `.cpp` or `.inl` defines its functions in the header's declaration order, with anonymous-namespace helpers above them. Compare the two files side by side.
- Includes are written from the module's include root. Core's public headers live under `Public/Core/…`. `Private/` mirrors `Public/` without the module folder.
- A `.cpp` includes its own header first.
- By-value parameters are `const` in both the declaration and the definition (CLAUDE.md § Code style). The scan can't see this, so compare each changed signature in the `.h` and the `.cpp`.
- Every header is self-sufficient: it includes what it uses (for example, `<string_view>` for `std::string_view`).
- No unused includes. No unused forward declarations. In headers, prefer a forward declaration when only a pointer or reference is used.
- New files live under `Source/<Module>/`, never in `Intermediate/`. Remind Fares to run `Setup.bat`.

### 6. Formatting (CLAUDE.md § Code style, `.editorconfig`)
- Allman braces everywhere, Lua tables included. Tabs for indentation. UTF-8 without BOM. No trailing whitespace. A final newline.
- The scan covers all of these. Confirm, then group repeats into one finding per file with the line list.

### 7. Warnings that fail the build (CLAUDE.md § Build system)
Vertex builds at `/W4` with warnings as errors, plus C4062. Each item below is a build break, sometimes in one configuration only. Name the warning number.
- A `switch` over an enum has no `default:` and handles every value (C4062). A function that returns from every `case` still needs a return after the `switch` (C4715).
- A parameter or local shadows a member (C4458, C4456, C4457). This is the reason for the `In` rule.
- A variable used only inside an assert is unused when asserts compile out (C4189 in Shipping).
- An unreferenced parameter (C4100). Omit the name or use `[[maybe_unused]]`.
- A narrowing conversion (C4244, C4267). Cast explicitly, for example `static_cast<int>(Message.size())`.

### 8. Defines and export macros (CLAUDE.md § Defines available to C++)
- `VERTEX_ENABLE_ASSERTS` is always defined, as `1` or `0`. Test it with `#if`. An `#ifdef` is always true.
- Nothing with a side effect goes inside an assert expression. It vanishes in Shipping (ERROR).
- Public classes and free functions in a `Public/` header carry **their own** module's API macro (`CORE_API` in Core, `RUNTIME_API` in Runtime). Templates, inline functions, and `constexpr` don't. The wrong module's macro is an ERROR: it compiles today and breaks the day modules become DLLs.

### 9. Logging
- Engine code logs through `Vertex::Log` with a fitting `ELogLevel`. No `printf` or `std::cout` outside Core's logging backend.
- An early return on bad input or configuration logs why, instead of failing silently.

### 10. Build files (when the change touches `.lua` or `.bat`)
- `Vertex.lua` and every `*.Module.lua` are pure data. They `return` a table and never call a premake, `os`, or `io` API.
- Everything premake-specific lives in `Build/Premake/`.
- Third-party sources are pinned by tag and verified commit SHA, never a branch.
- premake stays pinned. Flag an upgrade.

### 11. Documentation (CLAUDE.md § Code style, Comments)
CLAUDE.md holds the tag tables. Check against them; don't restate them here.
- **Coverage (ERROR).** Every class, struct, union, function, namespace- or class-scope variable, enum, and enum value declared in a header has a comment. The scan can't see a missing one, so read every changed header declaration.
- **Nothing extra (ERROR).** Namespaces, type aliases, macros, concepts, locals, and function bodies carry no comment, and a `.cpp` or `.inl` carries none besides its copyright line. The scan catches the `.cpp`/`.inl` case. Read the bodies of inline functions in headers yourself.
- **Header comment (ERROR).** Present exactly when the header holds two or more top-level classes, structs, or unions, or none. It never lists the file's contents.
- **Tags (ERROR).** Functions: summary, then `@tparam`, `@param`, `@return` (every non-void function), `@warning`, `@note`, each where it applies, in that order. An override, and a `= delete` function, has a one-line comment only. Classes, structs, unions: summary, then `@inherits` per direct base, the four always-required tags, and the conditional ones that apply, in table order. Enums: what it's for, and each value's meaning. A serialized enum carries the append-only `@warning`.
- **Format (ERROR).** The scan checks the mechanical forms: `/** … */` on one line, the block shape, comments on their own lines, no `//` except the copyright line, no `@info`, a blank line after every enum value but the last, a blank line before every commented declaration, and none after an access specifier.
- **Content (WARNING).** A comment that restates the code instead of saying why.
- A comment the change made wrong is stale: a WARNING.
- Every comment finding is `Owner: Claude`. Fix them before the next pass; they block "Review passed" like any other ERROR.
- **Doc staleness (ERROR).** If the change adds or renames a module, command, define, folder, or convention, CLAUDE.md and `README.md` say so in the same change. A new module updates the module graph and the README table.

### 12. Tests (CLAUDE.md § Tests)
- **Coverage (WARNING).** A change to Core's behavior ships with tests that prove it. Name the behavior left unproven.
- **Placement (ERROR).** Tests live in `Source/Tests/<Module>Tests/`, never in the module they test. A test file mirrors the header it tests and is named `<File>Tests.cpp`.
- **Names (WARNING).** Test cases read `"<Subject>: <expected behavior>"`.
- **`REQUIRE` (WARNING).** Used only where the rest of the test can't run after a failure. Elsewhere, `CHECK`.
- **Configurations (WARNING).** A test whose result depends on `VERTEX_ENABLE_ASSERTS` or the configuration still passes in Shipping.

### 13. Git hygiene (CLAUDE.md § Git workflow)
- The branch name fits the scheme (`feature/<milestone>-<topic>`, `fix/`, `chore/`, `docs/`), and the work isn't on `master`.
- The change is one piece of work. Suggest splitting unrelated work, such as tooling on a feature branch, into its own branch.
- Nothing generated is staged: `Binaries/`, `Intermediate/`, `Vertex.sln`, `*.vcxproj`, `ThirdParty/*/Source/`.

---

# Pass 2: Principal Engineer Review

Review like a principal engine programmer: correctness first, then design, scalability, and performance. Every issue cites `file:line`. Omit sections with no findings, except the two marked "always".

### What's good (always)
One to three specific strengths, and why each matters.

### Architecture & boundaries
- Does the code live in the right module and Runtime folder?
- Would it block a dedicated server without Render, a second graphics backend, or runtime-loaded plugins? Those are the promotion triggers, so they're the futures Vertex is designed for.
- The platform seam: would swapping raylib for another backend touch anything outside `Platform/Raylib/`?
- Public surface: what's in `Public/` that could be in `Private/`? Every public symbol is a promise to consumers.
- Nothing is ported: the earlier Vertex survives only as a skeleton (CLAUDE.md § The earlier Vertex). Flag code that looks pasted from elsewhere, UE above all (CLAUDE.md § Unreal Engine reference: study, never copy), and conventions the naming table has replaced, such as `V`-prefixed templates or a `TSize` alias.

### Ownership & lifetime
- For every new piece of state: who owns it, who may change it, and who needs to see it?
- Every resource (window, GPU handle, file) has an RAII owner. No naked `new`/`delete`. `std::unique_ptr` expresses single ownership. Raw pointers and references are non-owning.
- A `std::string_view` or `std::span` is never stored past the call unless its owner outlives it. It's never assumed to be null-terminated: pass it to C APIs with a length, the way `Log.cpp` uses `%.*s`.
- **Static libraries and globals.** Globals with constructors in different translation units initialize in an unspecified order. And the linker drops object files from a static library when nothing references them, so self-registering globals (future reflection, module registries) can silently vanish. Flag either pattern.
- Startup and shutdown order is explicit, and shutdown mirrors startup.

### Performance
Engine code runs every frame, so treat it as a budget.
- No heap allocation per frame in hot paths: `std::string` building, `std::vector` growth, `std::function` captures.
- `reserve` when the size is known. Heavy types go by `const&`. Cheap types (`std::string_view`, spans, small structs) go by value.
- Virtual calls and pointer chasing inside per-entity loops. Data layout when iterating many items.
- Logging in a per-frame path pays for formatting plus a flush on every line.
- Suggest what to measure (frame time, allocation count) instead of guessing.

### Errors & asserts
- Asserts are for programmer errors (broken invariants). Things that can happen on a player's machine (a missing file, a lost device) get a runtime check and a log line. Asserts vanish in Shipping.
- A result a caller must not ignore is `[[nodiscard]]`.

### Build configurations (Debug / Development / Shipping)
The defects that hurt most live in the configuration nobody builds locally.
- Both sides of every `#if VERTEX_*` block compile, and stub signatures match their declarations.
- Debug-only scaffolding stays out of Shipping.
- Without `--build`, name the configurations this change is at risk in.

### Thread safety (when the change touches threads or shared state)
- Shared mutable state has a documented owner or synchronization.
- raylib and GL calls happen on the main thread only.

### Domain checks (only for the areas the change touches)
- **Platform / window:** Vertex types in, backend translates; init and shutdown are symmetric; no backend state escapes.
- **Input:** an engine-owned key enum; the key mapping lives in the backend; switches over it have no `default:`.
- **Render / RHI:** backend-agnostic interfaces; opaque resource handles; no allocation per draw; the Null backend keeps compiling.
- **Scene / engine loop:** who owns each subsystem; update and render order; fixed vs variable timestep.
- **HeaderTool / reflection:** HeaderTool depends on Core only; generated code goes to `Intermediate/`; output is deterministic (stable ordering), so an unchanged header doesn't trigger a rebuild; registration survives static-library linking.

### Scalability & best practices (always)
- Patterns that need an edit per new feature (a `switch` case per key, a function per command). Propose the data-driven alternative.
- Point to the Unreal counterpart when it helps: UE 5.7.4 source at `C:\Developer Projects\HNDREDGAMES\UE_5_7_4\Engine\Source\`. Say what problem Epic solved and whether Vertex has that problem yet. **Study, never copy:** UE code is under Epic's EULA.
- If the better approach is out of scope, suggest it as a follow-up for a named milestone.
- If the milestone plan is in context, flag work that jumps ahead of the current milestone. The branch name carries the milestone (`feature/m0-…`).

---

# Convention governance

CLAUDE.md § "Decided when first needed" lists conventions that aren't settled yet: concepts, interfaces, global variables, and type aliases. When the change introduces one of these, or anything the naming table doesn't cover, or the code is inconsistent where the contract is silent (for example, `const` on by-value parameters in one function but not the next):

- Don't accept it silently, and don't invent a rule.
- Propose two or three options, recommend one with the reason, and ask Fares to choose.
- Once he chooses, offer to record the decision in CLAUDE.md § Code style, so the next review enforces it.

---

# Output format

```
# Vertex Review: <scope> on <branch>
**Built:** Debug ✓ · Shipping ✓   |   Not built (run `/review --build`)

## Standards & Style

**Result:** PASS | FAIL
**Files reviewed:** <n>
**Issues:** <n> (<x> errors, <y> warnings, <z> info)
**Style scan:** <n> candidates, <m> confirmed, <k> dropped as false positives

### <Category>

#### ERROR | WARNING | INFO: <short description>
**File:** `path/to/file.cpp:line`
**Rule:** CLAUDE.md § <section>, or "Principal" when no contract rule covers it
**Why:** the reason behind the rule, in one or two sentences
**Fix:** guidance or a short snippet. Owner: Fares | Claude

---

## Principal Engineer Review

**Overall:** Strong | Needs Work | Risky | Blocked
- <risk theme, three at most>

### What's good
### Must fix (blockers)
### Should fix
### Nice to have
### <Domain sections with findings>
### Convention questions
### Suggested order
<three to six steps: which finding to fix first, and why>
```

- Group repeated violations of one rule in one file into a single finding with a line list.
- Snippets illustrate the fix. They're short and never the full implementation.

If nothing is found:

```
# Vertex Review: <scope> on <branch>

## Standards & Style
**Result:** PASS · **Files reviewed:** <n> · **Issues:** 0

## Principal Engineer Review
**Overall:** Strong

### What's good
<specific strengths>
```

## Severity

- **ERROR:** breaks a CLAUDE.md rule, or fails the build in any configuration. Fix before committing.
- **WARNING:** a real risk or a deviation from best practice. Should fix.
- **INFO:** a suggestion. Optional.

## Tone

- A patient senior colleague. Never "obviously", "simply", or "just".
- Specific praise when earned, specific correction when needed.
- Bullets for findings, short prose for reasons. Concrete, never "consider improving".
- Never rewrite files unless asked.

## Passing the review

This review is step 4 of CLAUDE.md § Feature workflow.

- While any ERROR or WARNING remains, end by asking Fares which finding he wants to start with. Don't apply fixes until he asks.
- After his fixes, review the same scope again. Confirm each earlier finding is fixed, and check that the fixes didn't introduce new ones.
- The final pass always builds (`--build`). Don't pass code you haven't seen compile.
- When no ERROR or WARNING remains and the build is clean, say **"Review passed."** That's the go for step 5 (PR and merge). INFO findings stay optional. List any Fares chose to skip, so the PR body can mention them.
