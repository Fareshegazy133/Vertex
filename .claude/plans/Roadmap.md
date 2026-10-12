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
| M1.7 | Numeric type aliases | Done | [m1-numeric-types](m1-numeric-types.md) | #20 |
| M1.2 | Memory and `TUniquePtr` | Done | [m1-unique-ptr](m1-unique-ptr.md) | #21 |
| M1.8 | `TArrayView` | Done | [m1-array-view](m1-array-view.md) | |
| M1.3 | `TArray` | Todo | | |
| M1.9 | `VStringView` | Todo | | |
| M1.4 | `VString` | Todo | | |
| M1.5 | Hashing, `TSet`, and `TMap` | Todo | | |
| M1.6 | `VName` | Todo | | |

### M1.7: Numeric type aliases

- **Planned and decided:** see [m1-numeric-types](m1-numeric-types.md). It gives Core 13 global aliases (`int8`…`uint64`, `intptr`, `uintptr`, `usize`, `float32`, `float64`) and 16 limits in `namespace Vertex`, and CLAUDE.md § Numeric types makes them the only way Vertex spells a number.
- **UE:** `FGenericPlatformTypes` in `Runtime/Core/Public/GenericPlatform/GenericPlatform.h`. `Runtime/Core/Public/HAL/Platform.h` publishes it as the global names `int32`, `uint8`, and so on.
- **Not `String`:** `VString` is M1.4. An alias of `std::string` would spread std's API (`size()`, `c_str()`) that `VString` would then have to copy or break.

### M1.2: Memory and `TUniquePtr`

- **Planned and decided:** see [m1-unique-ptr](m1-unique-ptr.md). `Core/Memory/UniquePtr.h` holds `TUniquePtr`, a move-only owner the size of a raw pointer, and `Vertex::MakeUnique`. A derived owner moves into a base one only when the base has a virtual destructor, which std and UE don't check. `*` and `->` assert on an empty owner, and a type that's only declared fails to build where it would be deleted.
- **Conventions it set (CLAUDE.md):** a pointer type's `Get()` may return `nullptr`, the Getters row's one exception; `template <` with a space; a class template defines its member functions below the class, in declaration order; `noexcept` only on moves; `TUniquePtr` instead of `std::unique_ptr`.
- **UE:** `Runtime/Core/Public/Templates/UniquePtr.h`, and `FMemory` in `Runtime/Core/Public/HAL/UnrealMemory.h`.
- **Tooling, Claude:** StyleScan gained its function-order check here.

### M1.8: `TArrayView`

- **Planned and decided:** see [m1-array-view](m1-array-view.md). `Core/Containers/ArrayView.h` holds `TArrayView`, a view of a pointer and an `int32` count. It's 16 bytes and trivially copyable, so it's passed by value. `TConstArrayView` is the read-only form, and `Vertex::MakeArrayView` deduces the element type. Implicit constructors take a pointer and a count, a C array, another view, or any container with `data()` and `size()`. Every way in rejects a base view over derived elements, and `Slice`'s range check can't overflow.
- **Conventions it set (CLAUDE.md):** `int32` sizes and indices, with `Vertex::IndexNone` in `NumericTypes.h`; a read-only view is written `TConstArrayView<T>`; a container's `GetData()` joins the Getters exception; containers pair `Begin()`/`End()` with `begin()`/`end()`; value template parameters are PascalCase; an `if` skips its braces only for a one-line `return`; and an `if`, `for`, `while`, or `switch` has a blank line before and after it.
- **UE:** `Runtime/Core/Public/Containers/ArrayView.h`.
- **Tooling, Claude:** StyleScan flags `TArrayView<const`, a braceless `if` that doesn't return, and a control statement without its blank lines.

### M1.3: `TArray`

- **UE:** `Runtime/Core/Public/Containers/Array.h`.
- Growth policy, move semantics, iterators, and bounds checks through the asserts.
- Uses M1.8's size type. It converts to a `TArrayView` and accepts one: construction and `Append`, for example.
- **Decided in M1.8 ([m1-array-view](m1-array-view.md)):** sizes and indices are `int32`, and `Find` returns `Vertex::IndexNone`. `Array.h` adds `TArray`'s implicit conversion to a view (writable and `TConstArrayView`) and its own `Vertex::MakeArrayView` overload, so `ArrayView.h` never names `TArray`. `TArray` offers `Begin()`/`End()` beside `begin()`/`end()`, and `GetData()` may return `nullptr` when it's empty (CLAUDE.md § C++ naming).
- **From M1.8:** constructing from a raw pointer needs the view's stride guard (`std::is_convertible_v<TOther(*)[], TElement(*)[]>`), so a `TArray<VBase>` never takes `VDerived` elements. That's the guard's third use, which may raise the concept-naming fork (CLAUDE.md § Decided when first needed). A range check adds no two `int32`s (CLAUDE.md § Numeric types, Traps).
- **Fork found in M1.2: how it grows.** Move each element into the new storage, which needs `noexcept` moves (`TUniquePtr` has them), or copy the raw bytes, as UE's `TArray` does, which needs element types that survive being moved bitwise. A `TArray<TUniquePtr<…>>` test proves whichever path is chosen.
- **Deferred from M1.2:** `Malloc`/`Realloc`/`Free` wrappers, like UE's `FMemory`. `TArray`'s uninitialized storage is their first real caller, so planning decides here whether it needs them.

### M1.9: `VStringView`

- **UE:** `TStringView` in `Runtime/Core/Public/Containers/StringView.h`. It doesn't include `FString`'s header; `FString`'s API takes views.
- A non-owning view of characters: a pointer and a length, not null-terminated.
- It comes before `VString`, whose API takes views.
- **Forks to decide:**
  1. **The encoding and the character type.** Moved here from M1.4. UTF-8 is the likely answer; then `char` or `char8_t`.
  2. **Built on `TArrayView`, or standalone.**
  3. **Replacing `std::string_view` in `Log.h` and `Assert.h`:** now or later. `std::format` needs a formatter for `VStringView` either way.
- **Trap to document:** a view isn't null-terminated, so passing it to a C API such as raylib's needs a copy.
- **Deferred from M1.8:** `Left`, `Right`, and `Mid`, which clamp instead of asserting, like UE's (`Containers/ArrayView.h:433`). `TArrayView` has only `Slice`, which asserts. Add them here if `VStringView` is built on `TArrayView` (fork 2), or for the first caller that wants them.

### M1.4: `VString`

- **UE:** `FString` in `Runtime/Core/Public/Containers/UnrealString.h`.
- An owning string. Its encoding comes from M1.9, and its API takes `VStringView`.

### M1.5: Hashing, `TSet`, and `TMap`

- **UE:** `Runtime/Core/Public/Templates/TypeHash.h`, `Runtime/Core/Public/Containers/Set.h`, and `Map.h` next to it.
- **Deferred from M1.2:** comparison operators and a hash for `TUniquePtr`. It has none today, because comparing two owners only tells whether both are empty. Add them if a `TSet` or `TMap` needs to be keyed by an owner, or owners need sorting.

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
- **From M1.2:** a public header can hide backend state behind a `TUniquePtr` to a type it only declares (pimpl). The owning class then declares its destructor and move operations in its header and defines them in its `.cpp`, where the type is complete. Otherwise the build fails with C2027.

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
| **Math:** vectors and the like, plus float constants such as epsilon and infinity (deferred from M1.7; `std::numeric_limits` covers them until then) | The window or render work first needs it |
| **A call stack in the assert Error line** | The first assert that's hard to trace from its file and line |
| **`VX_CHECK_ALWAYS`**, which reports every failure, not only the first | The first real need |
| **Per-module precompiled headers.** `<format>` costs about 0.3 s per file that includes `Log.h` or `Assert.h`. `UniquePtr.h` brings it in too, through `Assert.h` (M1.2), and M2's Runtime headers will include that one widely. | Core's headers spread, or measured build-time pain |
| **Replacing the global `operator new` and `operator delete`,** so every allocation, `MakeUnique`'s included, can be tracked. UE does it with `REPLACEMENT_OPERATOR_NEW_AND_DELETE` (deferred from M1.2). | The first memory budget or leak hunt |
| **An array form, `TUniquePtr<T[]>`** (deferred from M1.2). `TUniquePtr` rejects array types today. | A heap buffer that `TArray` can't serve |
| **Custom deleters for `TUniquePtr`** (deferred from M1.2). On MSVC, an empty deleter needs `[[msvc::no_unique_address]]` or a private base to take no space: the standard `[[no_unique_address]]` is ignored. | The first object that isn't freed with `delete`, such as one from a pool or a C library |
| **Named concepts for the constraints written out by hand:** `TUniquePtr`'s guard (four times), and `TArrayView`'s stride guard (three times) and container check (twice, from M1.8). Naming the first concept is a convention fork (CLAUDE.md § Decided when first needed). | A third use of one constraint: `TArray` (M1.3) reusing the stride guard is one |
| **An `ElementType` alias on `TUniquePtr`** (deferred from M1.2) | The first generic code that reads a pointer's element type |
| **A 64-bit view, like UE's `TArrayView64`** (deferred from M1.8). `TArrayView` counts with `int32`, so it stops at about 2.1 billion elements. | A byte buffer over 2 GB |
| **`operator==` on `TArrayView` that compares elements** (deferred from M1.8). `std::span` leaves it out, because "equal" could mean the same memory or the same elements. | The first caller that compares two lists |
| **A `MakeArrayView` overload for `std::initializer_list`** (deferred from M1.8). Left out because the list's elements die at the end of the statement, so a stored view dangles at once. | The first API that wants a braced list |
| **`constexpr` on `TArrayView`** (deferred from M1.8) | The first view built at compile time, such as a lookup table |
| **Reverse iteration and sorting** on views and containers, like UE's `rbegin` and `Sort` (deferred from M1.8) | The first caller that needs them |
| **Limits for `usize`, `intptr`, and `uintptr`** (deferred from M1.7). M1.8's `int32` policy didn't need one. | The first code that needs one |
| **A standalone `Game` launcher.** Runtime never depends on Editor. | A game needs to ship without the editor |
| **Promoting a Runtime folder to a module** | One of the triggers in CLAUDE.md § Runtime layering |
| **`NOMINMAX` for Vertex modules,** so `<windows.h>` stops defining the `min` and `max` macros (deferred from M1.7). Build setup, so Claude writes it. | The first Vertex file that includes `<windows.h>` |
| **A naming rule for type aliases.** Fares chose none in M1.7: each alias is named when it's written. | Fares asks for one |
| **A Linux target.** Clang will warn on `UniquePtrTests.cpp`'s move-into-itself test (`-Wself-move`), which MSVC accepts (found in M1.2). | Not set |

## Tooling ideas

Claude chores, offered but not scheduled. Each one gets a `chore/` branch when Fares wants it.

| Idea | Trigger |
|---|---|
| A StyleScan check that a Runtime private header never shares a public one's path | Fares asks |
| A Setup/Clean guard that fails on stray `.h`/`.cpp` files under `Intermediate/` | Fares asks |
| A `.clang-format` (Allman, tabs) for format-on-save | Fares asks |
