# M1.2: Memory and `TUniquePtr`

**Status:** approved 2026-10-11
**Branch:** `feature/m1-unique-ptr`
**Roadmap:** M1, Core types

## The problem

Core has no way to say "this object owns that one." Today the choices are a raw `new`/`delete`, which leaks on an early return, or `std::unique_ptr`, which brings std naming and no Vertex asserts.

Three upcoming features need an owner:
- **M2's engine loop** owns the window and the backend chosen at startup (raylib or Null). It holds them through a pointer to their base class.
- **The raylib rule** keeps raylib types out of `Public/` headers. A Runtime class can hide its backend state behind a pointer to a type the header only declares (the "pimpl" pattern). That pointer has to own the state.
- **M1.3's `TArray`** moves its elements when it grows. A move-only element is the case that proves its move path works.

## How this feature solves it

When this feature is done:
- `Core/Memory/UniquePtr.h` holds `TUniquePtr<T>`. It owns one heap object and deletes it when it dies. It can be moved but not copied, and it's the size of a raw pointer.
- `MakeUnique<T>(Arguments…)` creates the object and its owner in one step.
- Misuse fails early. Dereferencing an empty pointer asserts. Moving a derived object into a base without a virtual destructor fails to build, which `std::unique_ptr` and UE's `TUniquePtr` both allow.
- CoreTests proves each behavior in all three configurations.

## Decisions (Fares, 2026-10-11)

| Question | Decision | Why |
|---|---|---|
| What "Memory" covers | `TUniquePtr` and `MakeUnique` only | Nothing needs raw memory yet. `TArray` designs that seam with its first caller, in M1.3. Waiting costs nothing: a memory tracker can later replace the global `operator new` and `operator delete` in one file, and `MakeUnique`'s allocations go through it unchanged. UE does that with `REPLACEMENT_OPERATOR_NEW_AND_DELETE`. |
| Moving a `TUniquePtr<Derived>` into a `TUniquePtr<Base>` | Yes, but only when `Base` has a virtual destructor. Moving to a more `const` version of the same type needs none. | M2 holds the chosen backend through its base class. Without a virtual destructor, deleting through the base skips `Derived`'s destructor, which is undefined behavior. std and UE accept that move with zero warnings at `/W4 /WX`; Vertex's guard turns it into a build error (C2440). |
| An array form, `TUniquePtr<T[]>` | No | `TArray` owns arrays. UE writes the whole class a second time for this. |
| Custom deleters | No | Everything is freed with `delete` today. A deleter adds a template parameter to every use, and on MSVC the standard `[[no_unique_address]]` is ignored: the prototype measured 16 bytes instead of 8. |
| What `Get()` means | It returns the raw pointer, or `nullptr` when empty, as std and UE do. Pointer types are the Getters row's one exception. No `Find()`. | Fares' call. It reads the way anyone from std or UE expects, and it covers `TObjectPtr` and weak pointers later. |
| Where the header lives | `Core/Memory/UniquePtr.h`. The tests go in `Private/Memory/UniquePtrTests.cpp`. | Vertex's folders name a concern (`Debug`, `Logging`, `Types`), not a language feature. UE's `Templates/` holds 83 unrelated headers. `Core.Module.lua` already lists "memory" as one of Core's jobs. |

## Smaller choices Claude made (change any)

**Safety**
- **`*` and `->` assert when the pointer is empty (`VX_ASSERT`).** They hand back the object itself, so there's nothing sensible to return. The check compiles out of Shipping.
- **The guard covers both ways in:** the constructor from a raw pointer and the converting move. Otherwise `TUniquePtr<VBase>(new VDerived)` would slip past it. Moving from base to derived, or dropping `const`, never compiles.
- **The class rejects array types** with a `requires` clause, so `TUniquePtr<int32[]>` fails at the use with C7602. Without it, that type would compile and call `delete` where `delete[]` is needed.
- **One private helper deletes the object, and checks first that its type is complete.** The destructor, move assignment, and `Reset` all go through it. Without the check, deleting a type that's only declared is warning C4150: only `/WX` makes it an error, and the object's destructor silently doesn't run.
- **The pimpl rule goes in the destructor's `@warning`.** A class that owns a `TUniquePtr` to a declared-only type declares its destructor and move assignment in its header, and defines them in its `.cpp`, where the type is complete.
- **The old object is deleted last**, after the owner has let go of it, in move assignment and `Reset`. If its destructor reaches back into the owner, it finds the owner already in its new state. UE does the same (`UniquePtr.h:218`).
- **Moving a pointer into itself keeps the object.**

**API**
- **Constructing from `nullptr` is implicit**, so a factory can `return nullptr;`. `Ptr = nullptr;` then empties a pointer through the move assignment, with no extra operator.
- **The constructor from a raw pointer and `operator bool` are `explicit`** (the lesson from the old code). An implicit adoption would let a function that takes a `TUniquePtr` silently take ownership of a raw pointer passed by mistake. `IsValid()` sits beside `operator bool`, because it reads better inside an expression.
- **`Release()` hands the object to the caller; `Reset()` deletes it.** `Reset()` takes no pointer. UE's `Reset(T*)` takes a `T*`, so a derived pointer converts on the way in and the guard never sees it. Adopting a new raw pointer is `Ptr = TUniquePtr<VFoo>(Raw);`, which the guard checks.
- **The converting move takes the pointer through `Other.Release()`**, so two versions of the class never need to be friends.
- **`noexcept` goes on the two moves only.** Containers check it to decide between moving and copying. It never goes on a function that can fail an assert: CoreTests' handler throws through a failed assert (CLAUDE.md § Asserts), and `noexcept` would turn that into `std::terminate`.
- **`[[nodiscard]]` goes on `Release()` and `MakeUnique`.** Dropping either result leaks the object, and the prototype showed it fails the build (C4834).
- **No comparison operators.** Two owners never point at the same object, so comparing them only tells whether both are empty, and `IsValid()` answers that.
- **`MakeUnique` passes its arguments in parentheses**, so C++20 builds an aggregate from them: `MakeUnique<VSize>(800, 600)` works without a constructor.
- **No `ElementType` alias yet.** No code reads a pointer's element type today.

**Build and tooling**
- **No `CORE_API`.** It's a template defined entirely in its header, so each module compiles its own copy and there's nothing to export. UE's has none either.
- **`std::move` and `std::forward`** come from `<utility>`. Vertex has no `MoveTemp`.
- **The guard is a `requires` clause written on each of the two constructors, not a named concept.** Concepts stay on CLAUDE.md's "Decided when first needed" list, and naming the first one is a fork for another day.
- **Vertex code uses `TUniquePtr`, never `std::unique_ptr` or `std::make_unique`.** CLAUDE.md records it, and StyleScan flags them. No file uses them today.
- **StyleScan's function-order check lands in this feature's docs step.** The roadmap set this class as its trigger, and `UniquePtr.h` is the first real class to test it against. It can move to its own `chore/` branch if you prefer.

## How it fits

`UniquePtr.h` is header-only, so `VertexCore.lib` gains no code. It includes `Core/Debug/Assert.h` for its two asserts, and `<cstddef>`, `<type_traits>`, and `<utility>`.

```
Core/Debug/Assert.h
  ↑ Core/Memory/UniquePtr.h
      ↑ CoreTests                          (this feature)
      ↑ TArray's tests                     (M1.3: a move-only element)
      ↑ Runtime's engine loop and backends (M2: owned through their base class)
```

Runtime, Editor, and HeaderTool reach it through Core, which is a public dependency.

## UE references (study, never copy)

All under `Runtime/Core/Public/`.
- `Templates/UniquePtr.h:105`: the class. It inherits privately from its deleter, so an empty deleter takes no space. Vertex has no deleter.
- `Templates/UniquePtr.h:134`: the constructor from a raw pointer, `explicit` and templated on the source type. Vertex takes both, and adds the virtual-destructor guard.
- `Templates/UniquePtr.h:196`: the converting move. Vertex adds the guard, and uses `Release()` instead of a friend declaration.
- `Templates/UniquePtr.h:214`: move assignment. The comment at line 218 explains deleting the old object last. Vertex takes the order.
- `Templates/UniquePtr.h:300` and `:310`: `operator->` and `operator*`, with no null check. Vertex asserts.
- `Templates/UniquePtr.h:324`: `Get()`, which may return `nullptr`. That's the exception Fares chose.
- `Templates/UniquePtr.h:334` and `:346`: `Release()`, which is `[[nodiscard]]`, and `Reset(T*)`. Vertex's `Reset()` takes no pointer.
- `Templates/UniquePtr.h:55`: the comment about a `TUniquePtr` to a forward-declared type inside a `UObject`. It's the pimpl trap that Vertex's complete-type check catches.
- `Templates/UniquePtr.h:498`: the array form, the whole class written a second time. Vertex leaves it out.
- `Templates/UniquePtr.h:918`: `MakeUnique`.
- `Modules/Boilerplate/ModuleBoilerplate.h:46`: `REPLACEMENT_OPERATOR_NEW_AND_DELETE`, which sends every `new` to `FMemory`. It's the path Vertex takes when memory tracking arrives.
- `HAL/UnrealMemory.h:93`: `FMemory`, deferred by fork 1.

## Files

| File | What changes | Who |
|---|---|---|
| `Core/Public/Core/Memory/UniquePtr.h` | New: `TUniquePtr` and `MakeUnique` | Fares (comments: Claude) |
| `Tests/CoreTests/Private/Memory/UniquePtrTests.cpp` | New: the tests and the compile-time proofs | Fares |
| `CLAUDE.md` (repo root) | The docs step: see After the cards | Claude |
| `.claude/skills/review/SKILL.md` | The same rules, for `/review` | Claude |
| `.claude/skills/review/Scripts/StyleScan.sh` | The function-order check, and a flag on `std::unique_ptr`/`std::make_unique` | Claude |

The `Core/` and `Tests/` paths are under `Source/`. Create new files on disk, then run `Scripts\Setup.bat`.

## Cards

Each card gets a full walkthrough when we reach it. `/implement-feature` ticks a card once it builds and passes review. Each card adds its own tests, and some end with a build that must fail, which is undone before moving on.

- [ ] **Card 1: The owner.** Create `UniquePtr.h`: the class, with array types rejected, the default and `nullptr` constructors, the `explicit` constructor from a raw pointer, the deleted copy, the destructor and its complete-type check, `Get()`, `IsValid()`, `explicit operator bool`, and `MakeUnique`. Create the test file with tests 1–3. Then watch three builds fail: an array type, a declared-only type, and an implicit adoption.
  *Why here:* every later card moves, hands over, or reaches the object this card owns.
- [ ] **Card 2: Reaching the object.** `operator*` and `operator->`, each asserting when the pointer is empty. Test 4.
  *Why here:* the later cards' tests read the object through `->`. It's also the first function that must not be `noexcept`.
- [ ] **Card 3: Handing it over.** `Release()` and `Reset()`, each letting go of the object before anything is deleted. Tests 5 and 6. Then watch an ignored `Release()` fail the build.
  *Why here:* the moves in card 4 are built on `Release()`.
- [ ] **Card 4: Moving it.** The move constructor and move assignment, both `noexcept`, with the old object deleted last and a move into itself made safe. Tests 7–9, and the compile-time proofs that it can't be copied and that its moves are `noexcept`.
  *Why here:* it needs `Release()` from card 3.
- [ ] **Card 5: Derived to base.** The guard, on the constructor from a raw pointer and on the converting move. Test 10, and the compile-time proofs of what the guard accepts and rejects. Then watch a move into a base without a virtual destructor fail the build.
  *Why here:* the guard sits on two constructors that exist by now, and it's the one idea that std's version doesn't have.

## Tests

In all three configurations, they prove:
1. `TUniquePtr`: is empty when default-constructed or made from `nullptr`.
2. `TUniquePtr`: `MakeUnique` passes its arguments to the constructor.
3. `TUniquePtr`: deletes its object when it's destroyed.
4. `TUniquePtr`: `*` and `->` assert when it's empty (in Debug and Development; Shipping has no assert to catch).
5. `TUniquePtr`: `Release` hands over the object without deleting it.
6. `TUniquePtr`: `Reset` lets go of the object, then deletes it.
7. `TUniquePtr`: a move leaves the source empty and the object alive.
8. `TUniquePtr`: move assignment takes the new object, then deletes the old one.
9. `TUniquePtr`: move-assigning to itself keeps the object.
10. `TUniquePtr`: deletes a derived object through its base.

And at compile time (`static_assert`, CLAUDE.md § Tests):
- It's the size of a raw pointer.
- A raw pointer doesn't convert to it implicitly, and it doesn't convert to `bool` implicitly. `nullptr` does convert to it.
- It can't be copied or copy-assigned, and both moves are `noexcept`.
- A derived pointer moves into a base one when the base has a virtual destructor, and into a more `const` one always. It doesn't when the base has no virtual destructor, from base to derived, or when the move would drop `const`. The same holds for the constructor from a raw pointer.

## How we know it works

- The whole solution builds with zero warnings in Debug, Development, and Shipping.
- `VertexCoreTests.exe` passes 25/25 in all three (the 15 existing tests and these 10).
- Each card's must-fail build fails with the expected error: C7602 for an array type, C2027 for a declared-only type, C4834 for an ignored `Release()`, and C2440 for a base without a virtual destructor. Each one is undone.
- StyleScan's function-order check passes on `UniquePtr.h` and fires on a test fixture with a misordered class.

Before writing this plan, Claude prototyped the whole shape in the scratchpad, with Vertex's flags in Debug and Shipping settings: the guard on both constructors, the array rejection, the complete-type check, `[[nodiscard]]`, the delete order, a move into itself, `return nullptr;`, an aggregate through `MakeUnique`, and the 8-versus-16-byte deleter measurement.

## After the cards

1. **Docs (Claude):**
   - CLAUDE.md § C++ naming, Getters row: a pointer type's `Get()` returns its raw pointer, or `nullptr` when empty, as std and UE do. It's the only `Get` that may come back empty, and its Why line gives the reason.
   - CLAUDE.md § Code style: `noexcept` goes on move constructors and move assignments, where containers check it, and never on a function that can fail an assert.
   - CLAUDE.md § Code style: Vertex code owns heap objects through `TUniquePtr` and `MakeUnique`, never `std::unique_ptr`, `std::make_unique`, or a naked `new`/`delete`.
   - `/review` (`.claude/skills/review/SKILL.md`): the Getters exception, the `noexcept` rule, and the Ownership bullet names `TUniquePtr` instead of `std::unique_ptr`.
   - StyleScan: the function-order check (CLAUDE.md § Declaration order), tested against `UniquePtr.h` and a misordered fixture. A flag on `std::unique_ptr` and `std::make_unique`.
   - Roadmap: the function-order idea comes off Tooling ideas.
2. **Review:** `/review`, until "Review passed".
3. **Ship:** `/ship-feature`, when Fares says so.

## Later (not in this feature)

- **`Malloc`/`Realloc`/`Free` wrappers, like UE's `FMemory`.** Trigger: `TArray` needs raw storage (M1.3), or memory tracking arrives.
- **Replacing the global `operator new` and `operator delete`,** so every allocation can be tracked. Trigger: the first memory budget or leak hunt.
- **An array form, `TUniquePtr<T[]>`.** Trigger: a heap buffer that `TArray` can't serve.
- **Custom deleters.** Trigger: the first object that isn't freed with `delete`, such as one from a pool or a C library. On MSVC, an empty deleter needs `[[msvc::no_unique_address]]` or a private base to take no space.
- **Comparison operators and hashing.** Trigger: a `TSet` or `TMap` keyed by `TUniquePtr` (M1.5), or sorting them.
- **A named concept for the guard.** Trigger: a third use of the constraint. Naming the first concept is a convention fork (CLAUDE.md § Decided when first needed).
- **An `ElementType` alias.** Trigger: the first generic code that reads a pointer's element type.
