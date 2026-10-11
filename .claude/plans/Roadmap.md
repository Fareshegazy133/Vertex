# Vertex roadmap

This file says what Vertex builds, in what order, and where each feature stands. It's the single source of truth for order and status. The workflow skills read it and keep it current (CLAUDE.md § Feature workflow).

## How to read it

- A **milestone** is a group of features with one goal. It's tagged on `master` (`m1`, `m2`, …) when its last feature merges.
- A **feature** is one run of the Feature workflow: one branch, one plan, one PR.
- **The build order runs top to bottom.** Milestones run in the order they appear here, and so do the rows in each table.
- **An ID never changes and is never reused.** A feature added later takes its milestone's next free number, and its row goes where it will be built. Plans, PRs, and conversations refer to these IDs, so renumbering would make old references point at the wrong feature.
- A feature's notes hold what its planning needs: the UE counterpart, the forks to decide, and lessons from the old code. Planning turns the forks into decisions in the feature's plan.
- UE paths are relative to `C:\Developer Projects\HNDREDGAMES\UE_5_7_4\Engine\Source\`. Study them; never copy them.

| Status | Meaning | Set by |
|---|---|---|
| Todo | Not planned yet | `/roadmap` |
| Planning | Being planned on its branch | `/plan-feature` |
| Planned | Plan approved, no card done yet | `/plan-feature` |
| In progress | Cards are being implemented | `/implement-feature` |
| Done | Merged into `master` | `/ship-feature` |
| Dropped | Won't be built. The row stays, so its ID isn't reused. | `/roadmap` |

---

## M0: The skeleton builds

**Goal:** the modules, the build layer, and the raylib boundary, proven before any engine code.

**Done** 2026-10-08, tagged `m0`. It was built before features had IDs: PRs #1–#14 hold its history.

---

## M1: Core types

**Goal:** the types every later system is built from, written fresh and each one proven by tests in CoreTests. Each feature uses only the ones above it.

| ID | Feature | Status | Plan | PR |
|---|---|---|---|---|
| M1.0 | CoreTests harness on doctest (chore) | Done | | #15 |
| M1.1 | Asserts and Logging v2 | Done | [m1-asserts](m1-asserts.md) | #17 |
| M1.2 | Memory and `TUniquePtr` | Todo | | |
| M1.3 | `TArray` | Todo | | |
| M1.4 | `VString` | Todo | | |
| M1.5 | Hashing, `TSet`, and `TMap` | Todo | | |
| M1.6 | `VName` | Todo | | |

### M1.2: Memory and `TUniquePtr`

- **UE:** `Runtime/Core/Public/Templates/UniquePtr.h`, and `FMemory` in `Runtime/Core/Public/HAL/UnrealMemory.h`.
- **Lesson from the old code:** the constructor from a raw pointer and the conversion to `bool` are both `explicit`.
- **Forks to decide:**
  1. **What "Memory" covers.** Recommendation: only `TUniquePtr` and `MakeUnique` for now. Allocator and `Malloc` wrappers wait for a trigger, such as memory tracking or a custom allocator for `TArray`.
  2. **The shape of `TUniquePtr`.** It's move-only. Open: moving a derived pointer into a base one, an array form (`TUniquePtr<T[]>`), and custom deleters (UE's deleter template parameter).
  3. **A naming clash.** CLAUDE.md's Getters row says `Get…` always succeeds and `Find…` may come back empty. But `Get()` on `std::unique_ptr` and on UE's `TUniquePtr` returns `nullptr` when empty. Options: `Get` asserts non-null and a `Find`-style accessor returns the maybe-null pointer, or the row gets an exception.
  4. **Type-alias naming** (CLAUDE.md § Decided when first needed). `TUniquePtr`'s element type needs it first, then `TArray`'s size type. `SetAssertHandler` used a trailing return type to put this off.
  5. **Asserts on misuse.** `VX_ASSERT` on a null dereference in `operator*` and `operator->`.
  6. **Where the header lives:** `Core/Templates/UniquePtr.h` (UE's folder) or `Core/Memory/UniquePtr.h`. The tests mirror it.
- **Tooling, Claude:** `TUniquePtr` is the first real class, which is the trigger for StyleScan's function-order check (see Tooling ideas).

### M1.3: `TArray`

- **UE:** `Runtime/Core/Public/Containers/Array.h`.
- Growth policy, move semantics, iterators, and bounds checks through the asserts.
- **Forks to decide:**
  1. **The index and size type policy.** The old code mixed an `int8` `INDEX_NONE` with an unsigned size.
  2. **The size alias's name.** Not `TSize`: it would read like a class template.

### M1.4: `VString`

- **UE:** `FString` in `Runtime/Core/Public/Containers/UnrealString.h`.
- An owning string with an encoding policy. UTF-8 is the likely answer.

### M1.5: Hashing, `TSet`, and `TMap`

- **UE:** `Runtime/Core/Public/Templates/TypeHash.h`, `Runtime/Core/Public/Containers/Set.h`, and `Map.h` next to it.

### M1.6: `VName`

- **UE:** `FName` in `Runtime/Core/Public/UObject/NameTypes.h`.
- Interned strings in one global table. A `VName` compares as one integer, and the table is thread-safe.
- **Lesson from the old code:** use C++20 `==` and `<=>`, not about 30 comparison overloads.

---

## M2: The application and its window

**Goal:** a real window and an engine loop, with the backend chosen at startup, so a headless Null backend can run where raylib can't.

**UE:** `Runtime/ApplicationCore/Public/GenericPlatform/GenericApplication.h` and `GenericWindow.h`; `FEngineLoop` in `Runtime/Launch/Public/LaunchEngineLoop.h`.

**Draft.** These features are a first cut. Settle the list, the order, and the split between `VApplication` and `VApplicationWindow` with `/roadmap` before planning M2.1.

| ID | Feature | Status | Plan | PR |
|---|---|---|---|---|
| M2.1 | `VApplication` and the engine loop | Todo | | |
| M2.2 | `VApplicationWindow` and its raylib backend | Todo | | |
| M2.3 | The Null backend, and choosing a backend at startup | Todo | | |
| M2.4 | raylib's log through `Vertex::Log` | Todo | | |
| M2.5 | An include-rule check in the build (chore) | Todo | | |

### M2.1: `VApplication` and the engine loop

- `VApplication` owns the engine's lifecycle (PreInit, Init, Tick, Exit, as in `FEngineLoop`) and absorbs `Vertex::InitializeEngine()`, including the shutdown it doesn't have yet.
- It ticks the simulation separately from rendering.
- Fares named the types `VApplication` and `VApplicationWindow` (2026-10-08).

### M2.2: `VApplicationWindow` and its raylib backend

- The window type is backend-neutral. The raylib translation lives in `Platform/Raylib/RaylibApplicationWindow.cpp` (CLAUDE.md naming row for backend files).

### M2.3: The Null backend, and choosing a backend at startup

- The Null backend is the headless mode for servers and tests. UE's counterpart: `Runtime/NullDrv/`.
- The command line picks the backend: `-backend=raylib|null`. Reading it needs UTF-8 `argv` on Windows, deferred from PR #11.
- Until this lands, only one backend may define the platform init function. A second one fails the link with LNK2005.

### M2.4: raylib's log through `Vertex::Log`

- raylib's `SetTraceLogCallback` hands every raylib log line to `Vertex::Log`, so the engine has one log.

### M2.5: An include-rule check in the build (chore)

- A build action that fails on the raylib rule and on the Runtime folder layering. It's build tooling, so Claude writes it.

---

## M3: Reflection (HeaderTool v0)

**Goal:** `VX_CLASS()` and `VX_PROPERTY()` markers become `*.generated.h` files, generated before each build, incrementally and deterministically.

- **UE:** `Programs/Shared/EpicGames.UHT/`.
- Type registration has to survive static-library linking. The linker drops an object file that nothing references.
- HeaderTool depends on Core only, and can use M1's containers.
- Deferred from M0: an exit-code enum, and diagnostics that MSBuild makes clickable.

Not broken down into features yet.

---

## M4: The object system and GC

**Goal:** `VObject`, `TObjectPtr`, and a mark-and-sweep garbage collector.

- **UE:** `Runtime/CoreUObject/Public/UObject/ObjectPtr.h` and `GarbageCollection.h`.
- The GC finds references by walking M3's reflected properties, which is why reflection comes first.
- `TObjectPtr` isn't an M1 core type: without objects and a collector, it would only be a raw pointer with a new name.

Not broken down into features yet.

---

## M5: Input

**Goal:** engine-owned `VKey` and `VMouseButton`. The raylib backend translates them in a private table.

- **UE:** `Runtime/InputCore/Classes/InputCoreTypes.h`.

Not broken down into features yet.

---

## M6: Render seam

**Goal:** a coarse-grained `VRHI` (begin, submit a draw list, end), with raylib and Null implementations.

- **UE:** `Runtime/RHI/Public/DynamicRHI.h`, and `Runtime/NullDrv/` for the Null one.

Not broken down into features yet.

---

## Unscheduled

Each item waits for its trigger. When one fires, `/roadmap` gives it an ID in the milestone that needs it.

| Item | Trigger |
|---|---|
| **Logging v3:** categories, a `VX_LOG(Category, Level, Format, …)` macro, arguments skipped when a category is filtered out, and stripping from Shipping. Sinks also make Log's output unit-testable. UE: `Runtime/Core/Public/Logging/LogMacros.h`. | Several systems need separate filtering |
| **Math:** vectors and the like | The window or render work first needs it |
| **A call stack in the assert Error line** | The first assert that's hard to trace from its file and line |
| **`VX_CHECK_ALWAYS`**, which reports every failure, not only the first | The first real need |
| **Per-module precompiled headers.** `<format>` costs about 0.3 s per file that includes `Log.h` or `Assert.h`. | Core's headers spread, or measured build-time pain |
| **A standalone `Game` launcher.** Runtime never depends on Editor. | A game needs to ship without the editor |
| **Promoting a Runtime folder to a module** | One of the triggers in CLAUDE.md § Runtime layering |
| **A Linux target** | Not set |

## Tooling ideas

Claude chores, offered but not scheduled. Each one gets a `chore/` branch when Fares wants it.

| Idea | Trigger |
|---|---|
| A function-order check in StyleScan (CLAUDE.md § Declaration order) | The first real class: M1.2's `TUniquePtr` |
| A StyleScan check that a Runtime private header never shares a public one's path | Fares asks |
| A Setup/Clean guard that fails on stray `.h`/`.cpp` files under `Intermediate/` | Fares asks |
| A `.clang-format` (Allman, tabs) for format-on-save | Fares asks |
