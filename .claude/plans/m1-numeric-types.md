# M1.7: Numeric type aliases

**Status:** done 2026-10-11
**Branch:** `feature/m1-numeric-types`
**Roadmap:** M1, Core types

## The problem

Core writes numbers the `std` way: `std::uint8_t` under both enums, `std::size_t` for buffer sizes, plain `int` in a test. Nothing is broken yet.

M1.2 and M1.8 are about to put sizes, counts, and indices into every container API. Without one vocabulary, each feature picks `int`, `std::int32_t`, or `std::size_t` on its own, and the mix spreads into every public header. The naming table has no row for these names either.

## How this feature solves it

When this feature is done:
- `Core/Types/NumericTypes.h` holds 13 global aliases (`int8`…`uint64`, `intptr`, `uintptr`, `usize`, `float32`, `float64`) and 16 limits (`Vertex::MaxInt32`, …).
- CoreTests proves at compile time that each alias is the type it claims to be.
- Every integer and float Vertex declares uses the aliases, and CLAUDE.md, `/review`, and StyleScan enforce it.

## Decisions (Fares, 2026-10-10)

| Question | Decision | Why |
|---|---|---|
| How integer aliases are spelled | Lowercase, as UE spells them: `int32`, `uint8` | They read like the built-in `int` and `std::int32_t`. Plain PascalCase (`Int32`) is how Vertex writes members and locals, so it would look like a variable. |
| Which aliases | `int8`, `int16`, `int32`, `int64`, `uint8`, `uint16`, `uint32`, `uint64`, `intptr`, `uintptr`, `usize`, `float32`, `float64`. No `isize`. | One vocabulary for every number Vertex declares |
| The `std::size_t` alias's name | `usize`. The u means unsigned. | `size` would clash with `size()` on containers and with `std::size()`. UE's `SIZE_T` is uppercase, and Windows declares a global `SIZE_T` (`basetsd.h:434`). The u tells a reader the value is unsigned. |
| Global, or inside `namespace Vertex` | Global | They appear on nearly every line, like `int`. A clash with another library fails loudly: redeclaring an alias as a different type is error C2371, and redeclaring it as the same type is legal. |
| Limits | 16 `constexpr` constants inside `namespace Vertex`: `Min` and `Max` for each of the eight integer types. Named `Max`/`Min` plus the type: `MaxInt32`, `MinInt8`, `MaxUInt64` (the unsigned ones spell `UInt`; see Added during implementation). | CLAUDE.md's Constants row says never `#define`. A macro ignores scope: in a prototype, `<windows.h>`'s `max` macro broke a correct `std::numeric_limits<int32>::max()` call (C4003). Unprefixed names live in `namespace Vertex`. |
| Where the header lives | `Core/Types/NumericTypes.h` (Claude's call; Fares approved the name) | Every file will include it, so its name must keep it small. "Types" rules out functions such as `Clamp`. Integers and floats travel together, so one header beats two. |
| When code uses them | Always, instead of `int`, `short`, `long`, `unsigned`, `float`, `double`, `std::size_t`, `std::uint8_t`, and the rest | One vocabulary, and the width is visible wherever data may later be saved or sent over the network |
| Exceptions to that | `int` stays for `main` and the exit code it returns, and for `printf`'s `%.*s` precision in `Log.cpp`. `char` stays: it holds text, not a number (M1.9 decides character types). A `std` type with no alias, such as `std::ptrdiff_t`, keeps its `std` name. | Something outside Vertex requires exactly that type |
| A naming rule for type aliases | None. Other aliases are named when they're written, until Fares asks for a rule. | Fares' call |

## Smaller choices Claude made (change any)

- **The limits are `inline constexpr`.** A plain `constexpr` at namespace scope gives every file its own private copy. When an inline function binds a reference to one (as `std::format` does with its arguments), the definitions in different files refer to different objects, which breaks the one-definition rule. `inline` makes it one constant shared by every file.
- **The limits come from `<cstdint>`'s `INT32_MAX` family**, not from `std::numeric_limits<…>::max()`. The header then needs no `<limits>`, and `<windows.h>`'s `min` and `max` macros can't touch it (the prototype hit exactly that).
- **The proofs live in CoreTests, as `static_assert`.** The integer checks can't fail while each alias names its `std` type, so in the header every file would compile them for nothing. In the test, they still catch anyone who repoints an alias. The float checks guard a real assumption: the standard promises neither that `float` is 32 bits nor that it uses IEEE 754.
- **The test count stays at 15.** Every proof here is a compile-time fact, so it's proven when the file compiles. A run-time `CHECK` of a `sizeof` would only repeat it.
- **No limits for `intptr`, `uintptr`, `usize`, or the floats yet.** The first three change size from platform to platform, and `std::numeric_limits` covers the floats until Math needs its own.
- **Each file that uses an alias includes `NumericTypes.h` itself**, even when another header already brings it in. That's how the code treats `<atomic>` and `<string_view>` today.
- **Moving the existing code over is a Claude card.** It's a mechanical edit (CLAUDE.md § Working mode). Say so if you'd rather type it.
- **The roadmap row becomes "Numeric type aliases".** The ID stays M1.7.

## How it fits

`NumericTypes.h` sits at the bottom of Core. It includes only `<cstddef>` and `<cstdint>`, and every other Vertex header can include it.

```
Core/Types/NumericTypes.h
  ↑ Core/Logging/Log.h, Core/Debug/Assert.h   (enum bases become uint8)
  ↑ Core's .cpp files                         (buffer sizes become usize)
  ↑ HeaderTool, CoreTests                     (through Core's public include path)
```

Runtime and Editor reach it through Core, which is a public dependency. raylib's headers never see it, and it never sees them.

## UE references (study, never copy)

- `Runtime/Core/Public/GenericPlatform/GenericPlatform.h:32`: `FGenericPlatformTypes`, the per-platform struct of `typedef`s. Lines 87–96 add `UPTRINT`, `PTRINT`, and `SIZE_T`. Epic needed the struct because old compilers had no `<cstdint>`. Vertex aliases the `std` types directly and leaves the struct out.
- `Runtime/Core/Public/HAL/Platform.h:1111`: publishes those types as global names. Vertex takes the lowercase names and the global scope.
- `Runtime/Core/Public/HAL/Platform.h:1208`: `static_assert`s on each type's size and sign, compiled in every file. Vertex puts its proofs in CoreTests instead.
- `Runtime/Core/Public/Math/NumericLimits.h:16`: the `MIN_int32`/`MAX_int32` macros, and `TNumericLimits` at line 72. Vertex takes the idea and uses `constexpr` instead.
- `Runtime/Core/Public/CoreTypes.h`: the catch-all header that `NumericTypes.h` avoids becoming.

## Files

| File | What changes | Who |
|---|---|---|
| `Core/Public/Core/Types/NumericTypes.h` | New: the 13 aliases and the 16 limits | Fares (comments: Claude) |
| `Tests/CoreTests/Private/Types/NumericTypesTests.cpp` | New: the compile-time proofs | Fares |
| `Core/Public/Core/Logging/Log.h` | `ELogLevel`'s base becomes `uint8` | Claude |
| `Core/Public/Core/Debug/Assert.h` | `EAssertKind`'s base becomes `uint8` | Claude |
| `Core/Private/Logging/Log.cpp` | `MaxLogLineLength` becomes `usize` | Claude |
| `Core/Private/Debug/Assert.cpp` | `MaxAssertMessageLength` becomes `usize` | Claude |
| `Core/Private/Logging/FormatToBuffer.cpp` | The writer's `Size` becomes `usize` | Claude |
| `Programs/HeaderTool/Private/HeaderToolMain.cpp` | The `argv` count cast becomes `usize` | Claude |
| `Tests/CoreTests/Private/Debug/AssertRecorder.h` | `Line` becomes `uint32` | Claude |
| `Tests/CoreTests/Private/BuildConfigurationTests.cpp` | The define counter becomes `int32` | Claude |
| `Tests/CoreTests/Private/Debug/AssertTests.cpp` | 10 `int` locals and constants become `int32`; `ExpectedLine` becomes `uint32` (added during card 4) | Claude |

All paths are under `Source/`. Create new files on disk, then run `Scripts\Setup.bat`.

## Cards

Each card gets a full walkthrough when we reach it. `/implement-feature` ticks a card once it builds and passes review.

- [x] **Card 1: The aliases.** Create `NumericTypes.h` with the 13 aliases, regenerate, and build.
  *Why here:* everything else in this feature uses them.
- [x] **Card 2: The limits.** Add the 16 `inline constexpr` constants inside `namespace Vertex`.
  *Why here:* they're declared with the aliases from card 1. They also bring the one new idea in this feature, a constant that a header shares with every file, so they get their own card.
- [x] **Card 3: The proofs.** Write `NumericTypesTests.cpp`: `static_assert`s that each alias is its `std` type, that the floats are 4 and 8 bytes and IEEE 754, and that each limit matches `std::numeric_limits`. Then break an alias on purpose and watch the build fail.
  *Why here:* it proves cards 1 and 2 before the rest of the code depends on them.
- [x] **Card 4: Move the existing code over (Who: Claude).** Replace the `std` and built-in uses listed in Files, and fix each file's includes. Claude makes the edits and walks you through the diff.
  *Why here:* the code switches only once the header is proven.

## Tests

In all three configurations, they prove at compile time:
1. Each integer alias is the `std` type it's named after: `int32` is `std::int32_t`, `intptr` is `std::intptr_t`, `usize` is `std::size_t`.
2. `float32` is `float` and `float64` is `double`. They're 4 and 8 bytes, and both are IEEE 754.
3. Each limit equals `std::numeric_limits`'s value for its type.

## How we know it works

- The whole solution builds with zero warnings in Debug, Development, and Shipping.
- `VertexCoreTests.exe` passes 15/15 in all three. The count doesn't change, because the new proofs are compile-time.
- Mutation check: change `int8` to `char`, then `float64` to `float`. Each time, `NumericTypesTests.cpp` fails to compile. Then restore the header.
- `VertexEditor.exe` still prints its startup lines, since logging was migrated.
- StyleScan's new check finds no `std` integer names, `short`, `long`, `unsigned`, or bare `float` or `double` left in Vertex code outside `NumericTypes.h`.

## After the cards

1. **Docs (Claude):**
   - CLAUDE.md § C++ naming: a row for the numeric aliases (names, global scope, the header), the usage rule and its exceptions, and the reasons.
   - CLAUDE.md § C++ naming, Constants row: a constant a header shares is `inline constexpr`, inside `namespace Vertex`. The Global variables row is about mutable state.
   - CLAUDE.md § Decided when first needed: type aliases come off the list, replaced by "No naming rule for other type aliases. Each is named when it's written, until Fares asks for one."
   - `/review` (`.claude/skills/review/SKILL.md`): the usage rule in § Naming, and type aliases removed from the undecided list in § Convention governance.
   - StyleScan: flag `std::` integer names (`std::size_t`, `std::uint8_t`, `std::uint_least32_t`, …), `short`, `long`, `unsigned`, `signed`, and bare `int`, `float`, and `double` outside `NumericTypes.h`. Skip comments and `main`. The `printf` precision cast in `Log.cpp` is an expected hit that the reviewer drops.
   - Roadmap: M1.2's alias-rule fork goes away. M1.8's "size alias name" fork is settled as `usize`. M1.8 gains a trap found while prototyping: `int32 < usize` compiles at `/W4` without a warning, and a negative `int32` becomes a huge unsigned value.
2. **Review:** `/review`, until "Review passed".
3. **Ship:** `/ship-feature`, when Fares says so.

## Added during implementation (2026-10-11)

- **Card 1:** Fares first pointed the eight integer aliases at built-ins (`signed int`, `unsigned long long`, …), as UE does. They now point at the `std` types, as the plan decided. On Linux, `std::int64_t` is `long`, so `long long` would have made `int64` a different type from `std::int64_t`.
- **Card 2:** Fares spelled the unsigned limits `MaxUInt8`…`MaxUInt64`, not `MaxUint8`. The Decisions table now matches. He also grouped all the minimums before all the maximums.
- **Card 3:** Fares added `MinUInt8`…`MinUInt64` (all 0), so every integer type has a named `Min` and `Max`. That makes 16 limits, not 12, and CoreTests proves all 16.
- **Card 4:** a search of the whole tree found 11 more uses in `AssertTests.cpp` (10 `int`s and one `std::uint_least32_t`), which the plan had missed. They fall under the same usage rule, so card 4 moved them over too. What remains is the plan's exceptions: `main`, `HeaderTool`'s exit code, and `printf`'s precision in `Log.cpp`.

## Added during review (2026-10-11)

- **The floats are pinned.** `NumericTypesTests.cpp` now checks that `float32` is `float` and `float64` is `double`, not only their size and format. C++23's `std::float32_t` is a distinct type that passes both of those checks. CLAUDE.md § Numeric types records the trap.
- **CLAUDE.md § Tests records the compile-time convention.** A fact the compiler knows is proven with a namespace-scope `static_assert` and adds no test case.
- **The roadmap's M1.7 notes point at this plan**, instead of the planning-time notes that said floats would stay `float` and `double`.

## Later (not in this feature)

- **`NOMINMAX` for Vertex modules,** which stops `<windows.h>` from defining `min` and `max`. Trigger: the first Vertex file that includes `<windows.h>`.
- **Limits for `usize`, `intptr`, and `uintptr`.** Trigger: the first code that needs one. M1.8's size policy may be it.
- **Float constants** (epsilon, infinity). Trigger: the Math item in Unscheduled.
- **A naming rule for type aliases.** Trigger: Fares asks for one.
