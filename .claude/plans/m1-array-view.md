# M1.8: `TArrayView`

**Status:** done 2026-10-11
**Branch:** `feature/m1-array-view`
**Roadmap:** M1, Core types

## The problem

A function that only reads a list of elements has two poor options today:
- **Take `const std::vector<T>&`.** Every caller must then hold that one container, and Vertex is replacing the std containers.
- **Take `(T* Data, int32 Count)`.** The two parameters can disagree, and nothing checks the bounds.

M1.3's `TArray` needs a view type for its API, for example to construct from one or to `Append` one. M1.9's `VStringView` may be built on it. Later, the renderer will take vertex lists the same way.

## How this feature solves it

When this feature is done:
- `Core/Containers/ArrayView.h` holds `TArrayView<T>`: a pointer and an `int32` count, 16 bytes, passed by value. It owns nothing.
- `TConstArrayView<T>` is the read-only form.
- A function that takes a view accepts a pointer and a count, a C array, another view, or any container with `data()` and `size()`, such as `std::vector` or `std::array`. From M1.3, it accepts a `TArray` too.
- `Vertex::MakeArrayView` builds a view without spelling its type.
- `[]` and `Slice` assert on bad indices. `Find` returns `Vertex::IndexNone` when nothing matches.
- A view of a base type over derived elements fails to build, because it would read the wrong memory.

## Decisions (Fares, 2026-10-11)

| Question | Decision | Why |
|---|---|---|
| The size and index type | `int32` for every count and index in views and containers. "Not found" is `Vertex::IndexNone`, which is -1. | MSVC catches lost bits (C4267, C4244) but not a changed sign: `int32 < usize`, `usize X = SomeInt32`, and an unsigned reverse loop all compile silently at `/W4 /WX`. With a signed count, `Num() - 1` on an empty view is -1 instead of a huge number, and an assert can catch a negative index. `uint16` saves no memory: padding keeps the view at 16 bytes. `uint32` keeps the bugs near zero. The costs: a limit of about 2.1 billion elements, and a cast where std hands over a `usize`, which the build forces (C4267). |
| Read-only views | `TArrayView<const T>`, with the alias `TConstArrayView<T>` | One class serves both. The alias makes the right spelling the short one: a `const TArrayView<T>` can still write its elements. |
| How a view is built | Implicit constructors, plus `Vertex::MakeArrayView` overloads that deduce the element type | A function that takes a view accepts what the caller holds, with no extra call. `MakeArrayView` builds a view variable without spelling its type, which the constructors can't deduce from a pointer. |
| Containers Vertex doesn't own | One generic constructor and one generic `MakeArrayView` for any type with `data()` and `size()` | They cover `std::vector`, `std::array`, `std::span`, `std::string_view`, and third-party containers without naming any of them. Per-type overloads would need eight declarations for `vector` and `array` alone, because a `const` container needs its own (C2664 otherwise). Build time is the same either way: `Assert.h`'s `<format>` already brings in nearly everything `<vector>` needs (134 vs 136 headers). |
| Vertex's own containers | Each owner's header adds its implicit conversion and its `MakeArrayView` overload. `TArray`'s go in `Array.h`, in M1.3. | `ArrayView.h` is written first and never names an owner: the owner knows the view, not the other way round. UE does the reverse: `ArrayView.h:14` includes `Array.h`, so every file that wants a view pays for `Array.h` too. |
| Where "no index" lives, and how it's spelled | `Vertex::IndexNone`, an `inline constexpr int32` equal to -1, in `Core/Types/NumericTypes.h` | It's a general sentinel, a "nullptr for integers" that any `Find` can return, so using it shouldn't mean including `ArrayView.h`. `NumericTypes.h` includes only `<cstddef>` and `<cstdint>`, and it already holds the limits, which are the same kind of thing. UE spells it as an anonymous enum because its code predates C++17's `inline` variables. With Vertex's flags, that enum broke three things a constant handles: `auto Index = INDEX_NONE; Index = 3;` (C2440), printing it through `std::format` (C2338), and `std::max(Index, INDEX_NONE)` (C2672). It would also break two naming rules: UPPER_SNAKE is for macros, and enums are `enum class`. |
| `GetData()` on an empty view | It returns `nullptr`. The Getters row's exception grows from a pointer type's `Get()` to cover a container's or view's `GetData()`. Every other getter on them follows the `Get`/`Find` rule. | An empty view has nothing to point at. A blanket exception for containers would lose the `Get`/`Find` split where it matters most: lookups, such as M1.5's `TMap::Find`. |

## Smaller choices Claude made (change any)

**Where things live**
- **`Core/Public/Core/Containers/ArrayView.h`.** The tests go in `Private/Containers/ArrayViewTests.cpp`. CLAUDE.md's examples already use `Core/Containers/`.
- **`TConstArrayView` sits directly below the class.** Vertex code writes it for every read-only view, and StyleScan flags `TArrayView<const` anywhere else.
- **The `MakeArrayView` overloads are free function templates in `namespace Vertex`**, defined where they're declared, like `MakeUnique`.

**Safety**
- **The stride guard.** Every constructor that takes elements from outside rejects a derived type into a view of its base. Such a view steps through memory at the base's size, so every element after the first is read from the wrong address. The prototype read the wrong field, with zero warnings.
  - The guard is `std::is_convertible_v<TOtherElement(*)[], TElement(*)[]>`, the check `std::span` uses. It allows adding `const` and nothing else. UE's version is at `ArrayView.h:21`.
  - The C-array constructor needs no guard. A reference to an array only binds when the element types match, give or take `const`.
- **The pointer-and-count constructor asserts** that the count isn't negative, and that a null pointer comes with a count of 0.
- **The generic constructor asserts that the container's `size()` fits in an `int32`**, then casts once. Callers never write the cast.
- **`[]` asserts that the index is inside the view; `Slice` asserts that its whole range is.** Both checks compile out of Shipping, like `TUniquePtr`'s.
- **Not const-propagating.** A `const TArrayView<T>` can still write its elements, just as an `int32* const` can. The read-only form is `TConstArrayView<T>`. The class comment says so.
- **The generic constructor accepts a temporary container**, so `Sum(MakeScores())` works: the temporary lives until the end of the statement. A view stored past that statement dangles, and `@lifetime` says so.
- **No `std::initializer_list` constructor.** Its elements are destroyed at the end of the statement, so `TConstArrayView<int32> View = {1, 2, 3};` would dangle at once. UE allows it and warns in a comment (`ArrayView.h:262`).

**API**
- **Read:** `Num()`, `IsEmpty()`, `GetData()`, `IsValidIndex(Index)`, `[]`, and `begin()`/`end()`. `begin()` and `end()` return raw pointers, so range-for works, and keep the lowercase spelling that range-for requires.
- **`Slice(Index, Count)`** returns a view of part of this one and asserts the range, like UE's `Slice` and `std::span::subspan`. UE's `Left`, `Right`, and `Mid` clamp instead; they wait for M1.9 or a caller.
- **`Slice` is `[[nodiscard]]`.** `View.Slice(0, 2);` on its own looks like it trims the view in place, but it does nothing. The attribute makes that a build error (C4834, checked).
- **`Find(Value)`** returns the first matching index, or `Vertex::IndexNone`. **`Contains(Value)`** asks whether one exists.
  - Both need `==` on the element type.
  - A member of a class template is only compiled when it's called, so a view of a type without `==` still works.
- **`[[nodiscard]]` on every `MakeArrayView`**, as on `MakeUnique`.
- **No `operator==`.** `std::span` leaves it out because "equal" could mean the same memory or the same elements.
- **No copy constructor or copy assignment is declared**, because the compiler's versions are right. The view is trivially copyable, and a `static_assert` proves it.
- **A view is passed by value:** `const TConstArrayView<int32> Values`. Windows x64 passes a 16-byte struct through a hidden pointer to a copy, so it costs about what passing a reference does.
- **No `constexpr`, and no `CORE_API`**, like `TUniquePtr`. It's a template defined entirely in its header.

**Build**
- **`ArrayView.h` includes only `Assert.h`, `NumericTypes.h`, `<type_traits>`, and `<utility>`.** The generic constructor names no std container, so it needs none of their headers.
- **Both checks are written out where they're used**, with no named concept, like `TUniquePtr`'s guard. The stride guard appears on three constructors; the container check appears on the generic constructor and the generic `MakeArrayView`. Concepts are still on CLAUDE.md's "Decided when first needed" list.
- **No `usize` limit.** The size check compares against `Vertex::MaxInt32`, so M1.7's deferred limits stay deferred.

## How it fits

`ArrayView.h` is header-only, so `VertexCore.lib` gains no code.

```
Core/Types/NumericTypes.h     Core/Debug/Assert.h
(gains Vertex::IndexNone)
            ↑                         ↑
        Core/Containers/ArrayView.h
        (TArrayView, TConstArrayView, MakeArrayView)
            ↑ CoreTests                  (this feature)
            ↑ Core/Containers/Array.h    (M1.3: TArray converts to a view, and adds its MakeArrayView)
            ↑ VStringView                (M1.9 decides whether it's built on TArrayView)
```

A function that only reads takes a view, and its callers pass whatever they hold:

```
int32 Sum(const TConstArrayView<int32> Values)
  ← int32 Scores[4]          the C-array constructor
  ← std::vector<int32>       the generic constructor
  ← TArrayView<int32>        the converting constructor (adds const)
  ← {Pointer, Count}         the pointer-and-count constructor
  ← TArray<int32>            TArray's conversion (M1.3)
```

Runtime, Editor, and HeaderTool reach it through Core, which is a public dependency.

## UE references (study, never copy)

All under `Runtime/Core/Public/Containers/`.
- `ArrayView.h:137`: the class. Its comment at `:128` warns that views don't propagate `const`. Vertex takes both.
- `ArrayView.h:144`: a `static_assert` that the size type is signed. `ContainersFwd.h:68` makes `int32` the default, and `:70` declares `TConstArrayView`. Vertex takes `int32` without the template parameter, and takes the alias.
- `ArrayView.h:21`: the "extra stars" comment on UE's stride guard. Vertex spells the same check the way `std::span` does.
- `ArrayView.h:173` and `:206`: the generic range constructors, one for views and one for everything else. Vertex has one, for types with `data()` and `size()`.
- `ArrayView.h:250`: the pointer-and-count constructor. It's a template, so the guard sees the real element type. Vertex takes that.
- `ArrayView.h:262`: the `std::initializer_list` constructor. Vertex leaves it out.
- `ArrayView.h:331` and `:396`: `RangeCheck` and `[]`.
- `ArrayView.h:426`: `Slice`, which checks its range. `:433` to `:459`: `Left`, `Right`, and `Mid`, which clamp instead. Vertex takes `Slice` only.
- `ArrayView.h:530`: `Find`, which returns `INDEX_NONE`.
- `Misc/CoreMiscDefines.h:150`: `INDEX_NONE`, an anonymous enum. Vertex takes the value and the idea, spelled as an `inline constexpr int32`.
- `ArrayView.h:759`: `begin()`, a raw pointer.
- `ArrayView.h:888`: `MakeArrayView` from a pointer and a size.
- `ArrayView.h:14` and `:934`: `ArrayView.h` includes `Array.h`, and defines `TArray`'s constructor from a view. Vertex reverses that direction.

## Files

| File | What changes | Who |
|---|---|---|
| `Core/Public/Core/Containers/ArrayView.h` | New: `TArrayView`, `TConstArrayView`, and `MakeArrayView` | Fares (comments: Claude) |
| `Tests/CoreTests/Private/Containers/ArrayViewTests.cpp` | New: the tests and the compile-time proofs | Fares |
| `Core/Public/Core/Types/NumericTypes.h` | `Vertex::IndexNone` | Fares (comments: Claude) |
| `Tests/CoreTests/Private/Types/NumericTypesTests.cpp` | A `static_assert` that `IndexNone` is an `int32` equal to -1 | Fares |
| `CLAUDE.md` (repo root) | The docs step: see After the cards | Claude |
| `.claude/skills/review/SKILL.md` | The same rules, for `/review` | Claude |
| `.claude/skills/review/Scripts/StyleScan.sh` | Flags `TArrayView<const` outside `ArrayView.h` | Claude |
| `.claude/plans/Roadmap.md` | M1.3's notes | Claude |

The `Core/` and `Tests/` paths are under `Source/`. Create new files on disk, then run `Scripts\Setup.bat`.

## Cards

Each card gets a full walkthrough when we reach it. `/implement-feature` ticks a card once it builds and passes review. Each card adds its own tests, and two end with a build that must fail, which is undone before moving on.

- [x] **Card 1: The view.** Create `ArrayView.h`: the class with its pointer and `int32` count, the default constructor, the pointer-and-count constructor with its two asserts, `Num()`, `IsEmpty()`, and `GetData()`. Create the test file with tests 1–3 and the size and trivially-copyable proofs.
  *Why here:* every later card reads through these.
- [x] **Card 2: Reading the elements.** `IsValidIndex()`, `[]` with its assert, and `begin()`/`end()`. Tests 4–7.
  *Why here:* every later card's tests read through `[]` and range-for.
- [x] **Card 3: Read-only views and the stride guard.** The `TConstArrayView` alias, and the constructor from another view, which can add `const`. The guard goes on that constructor and on the pointer-and-count one, which becomes a template so the guard can see the real type. Test 8 and the compile-time proofs. Then watch a view of a base type over a derived array fail to build (C2661).
  *Why here:* the constructors in cards 4 and 5 rely on the same idea. It's also the one mistake a hand-written view makes silently.
- [x] **Card 4: C arrays and `MakeArrayView`.** The C-array constructor, which reads the count from the array's type. Then `MakeArrayView` for a pointer and a count, and for a C array. Tests 9 and 10.
  *Why here:* `MakeArrayView` exists mainly for the pointer case. Since card 3, that constructor is a template, and a template constructor's parameter can't tell the compiler the view's element type.
- [x] **Card 5: Containers Vertex doesn't own.** The generic constructor (it checks for `data()` and `size()`, applies the guard, and asserts the size fits), and the generic `MakeArrayView`. Tests 11, 12, and 17, and the proofs of what it rejects.
  *Why here:* it's the most template-heavy piece, and it builds on card 3's guard and card 4's `MakeArrayView`.
- [x] **Card 6: Slicing and searching.** `Slice()`, `Find()`, and `Contains()`. `Vertex::IndexNone` goes in `NumericTypes.h`, with its proof in `NumericTypesTests.cpp`. Tests 13–16. Then watch an ignored `Slice` fail to build (C4834).
  *Why here:* they only use what the earlier cards built.

## Tests

In all three configurations, they prove:
1. `TArrayView`: is empty when default-constructed.
2. `TArrayView`: views a pointer and a count.
3. `TArrayView`: asserts on a negative count, or on a null pointer with elements. Debug and Development only.
4. `TArrayView`: `[]` reads and writes the viewed elements.
5. `TArrayView`: `[]` asserts on an index outside the view. Debug and Development only.
6. `TArrayView`: `IsValidIndex` accepts 0 to `Num() - 1` only.
7. `TArrayView`: range-for visits every element in order.
8. `TArrayView`: converts to a read-only view of the same elements.
9. `TArrayView`: views a C array and counts its elements.
10. `TArrayView`: `MakeArrayView` views a pointer and a count, or a C array.
11. `TArrayView`: views a `std::vector` and a `std::array`.
12. `TArrayView`: takes a temporary container as an argument.
13. `TArrayView`: `Slice` views part of the elements.
14. `TArrayView`: `Slice` asserts on a range outside the view. Debug and Development only.
15. `TArrayView`: `Find` returns the first matching index, or `IndexNone`.
16. `TArrayView`: `Contains` reports whether a value is in the view.
17. `TArrayView`: asserts when a container is too big for an `int32`. Debug and Development only (added during card 5).

Tests 3, 5, 14, and 17 are compiled out of Shipping. It has no asserts, and an out-of-range read there is undefined behavior.

And at compile time (`static_assert`, CLAUDE.md § Tests):
- It's 16 bytes and trivially copyable.
- A view converts to its read-only form, and not back.
- A derived element type is rejected for a base view: from a pointer, another view, a C array, and a `std::vector`.
- A `const` source is rejected for a writable view: a `const` C array and a `const std::vector`.
- `std::vector<bool>` and `std::initializer_list` aren't accepted.
- `MakeArrayView` returns `TArrayView<T>` for a writable source, and `TArrayView<const T>` for a `const` one.
- `Vertex::IndexNone` is an `int32` equal to -1 (in `NumericTypesTests.cpp`).

## How we know it works

- The whole solution builds with zero warnings in Debug, Development, and Shipping.
- `VertexCoreTests.exe` passes 43/43 in Debug and Development (the 26 existing tests and these 17), and 38/38 in Shipping, which compiles out tests 3, 5, 14, and 17.
- Each must-fail build fails with the expected error, then is undone: C2661 for a base view over a derived array (card 3), and C4834 for an ignored `Slice` (card 6).

Before writing this plan, Claude prototyped the whole class in the scratchpad against Vertex's real `Assert.h`, in Debug and Shipping settings. It checked:
- every constructor and both checks, including the out-of-line definition of the generic constructor;
- that all five asserts fire in Debug;
- both must-fail errors;
- the conversion warnings behind fork 1, and the 16-byte sizes.

## After the cards

1. **Docs (Claude):**
   - CLAUDE.md § C++ naming, Getters row: the exception also covers a container's or view's `GetData()`, which returns `nullptr` when it holds no elements. Every other getter on them follows `Get`/`Find`.
   - CLAUDE.md § Numeric types: containers and views count and index with `int32`, and "not found" is `Vertex::IndexNone`, an `inline constexpr int32` in `NumericTypes.h`, not UE's anonymous enum. A `usize` from std is cast once, where it enters. For views, the generic constructor does that cast. The why is the sign table from fork 1, and the three enum errors.
   - CLAUDE.md § Code style: a read-only view is written `TConstArrayView<T>`. A function that only reads a list takes one by value.
   - CLAUDE.md § C++ naming: a container offers `Begin()` and `End()` alongside the lowercase `begin()` and `end()` that range-for requires (Fares, card 2).
   - CLAUDE.md § C++ naming: a value template parameter is PascalCase with no prefix, like a constant: `template <int32 ArrayNum>` (card 4). The `T` prefix stays for type parameters.
   - `/review` (`.claude/skills/review/SKILL.md`): the same four rules.
   - StyleScan: flag `TArrayView<const` outside `ArrayView.h`.
   - Roadmap, M1.3's notes: `TArray` adds its implicit conversion to a view and its `MakeArrayView` overload in `Array.h`, its sizes are `int32`, and it offers `Begin()`/`End()` beside `begin()`/`end()`. It can reuse the stride guard, which may raise the concept-naming fork.
2. **Review:** `/review`, until "Review passed".
3. **Ship:** `/ship-feature`, when Fares says so.

## Added during implementation (2026-10-11)

- **Card 1:** Fares named the members `Data` and `Count`, and the pointer-and-count constructor's parameters `InData` and `InCount`.
- **Card 2, `Begin()` and `End()` (Fares).** The view offers PascalCase `Begin()` and `End()` beside the lowercase `begin()` and `end()` that range-for requires, so explicit calls read like the rest of Vertex. Claude raised the cost: two names for each function, which every later container repeats. UE has only the lowercase pair, plus `CreateIterator()` (`Array.h:3355`). Fares kept them, and the range-for test checks both. The docs step records the pair as a container convention.
- **Card 3:** Fares' first build kept the pointer-and-count constructor's parameter as `TElement*`, so `TOtherElement` couldn't be deduced and the constructor accepted nothing (C2661, with "could not deduce template argument"). He fixed it from the diagnostic's notes, as in M1.2's card 5.
- **Card 4, naming:** the naming table had no row for a value template parameter. `ArrayNum` follows the Constants row: PascalCase, no prefix. The docs step adds the row.
- **Card 5, test 17 (Claude):** the generic constructor's size assert had no test. A fake container whose `size()` reports one more than `Vertex::MaxInt32` proves it fires. It's an assert test, so Shipping compiles it out: 43 tests in Debug and Development, 38 in Shipping.
- **Card 5, constructor order:** StyleScan reads the converting constructor (`const TArrayView<TOtherElement>&`) as a copy constructor, the way `TUniquePtr`'s converting move sits with the moves. So the generic constructor goes above it: your own parameter lists, then copy, then move (CLAUDE.md § Declaration order). Card 5's walkthrough put it below by mistake, and card 6 moves it.

## Added during review (2026-10-11)

- **Fixed from the first pass:** `Find`'s loop index became an `int32`; two `static_assert`s in the tests spell `TConstArrayView<int32>`; and the two default-constructed views in the tests are `const`, not `constexpr`, because the view makes no `constexpr` promise.
- **Braces and blank lines (Fares).** The first pass asked whether a one-statement `if` needs braces. Fares set two rules, now in CLAUDE.md § Code style and checked by StyleScan:
  - an `if` skips its braces only when its one statement is a `return` and the whole `if` fits on one line, as in `Find`;
  - an `if`, `for`, `while`, or `switch` has a blank line before and after it, except where it opens or closes a block.
  - Fares said "function"; the rule reads "block", because `Find`'s one-line `if` is the only statement in its loop's body. A scan of the whole codebase found no other code that breaks either rule.
- **INFO Fares skipped:** `ArrayView.h:235` spells the C-array parameter `TElement(&Array)`, and the declaration `TElement (&Array)`. They compile the same.

## Later (not in this feature)

- **A 64-bit view, like UE's `TArrayView64`.** Trigger: a byte buffer over 2 GB.
- **`Left`, `Right`, and `Mid`, which clamp instead of asserting.** Trigger: M1.9's `VStringView`, or the first caller that wants them.
- **`operator==` that compares elements.** Trigger: the first caller that compares two lists.
- **A `MakeArrayView` overload for `std::initializer_list`.** Trigger: the first API that wants a braced list.
- **Named concepts for the stride guard and the container check.** Trigger: `TArray` (M1.3) needs the same checks, a third use. Naming the first concept is a convention fork (CLAUDE.md § Decided when first needed).
- **`constexpr`.** Trigger: the first view built at compile time, such as a lookup table.
- **Reverse iteration and sorting** (UE's `rbegin` and `Sort`). Trigger: the first caller that needs them.
