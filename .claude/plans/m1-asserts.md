# M1 feature 1: asserts and Logging v2

**Status:** approved by Fares 2026-10-08. Implementing, card by card.

Branch: `feature/m1-asserts`.

## Goal

Every M1 type after this one needs a way to say "this must be true, or the code has a bug". `TArray` will check its indexes, `TUniquePtr` its pointers, and so on. Those checks need readable messages ("Index 7 is out of range for 5 elements"), so `Vertex::Log` learns to format too.

When this feature is done:
- Core has `VX_ASSERT`, `VX_VERIFY`, and `VX_CHECK`, each with an optional message.
- `Vertex::Log` accepts a format and arguments: `Log(ELogLevel::Info, "Loaded {} files", Count)`.
- CoreTests proves every behavior in all three configurations.

## Decisions (Fares, 2026-10-08)

| Question | Decision |
|---|---|
| Which asserts? | All three, matching UE's `check`, `verify`, and `ensure`. |
| Their names? | `VX_ASSERT`, `VX_VERIFY`, `VX_CHECK` |
| What happens when one fails? | `VX_ASSERT` and `VX_VERIFY` log one Error line, then end the program. `VX_CHECK` logs and keeps going. |
| Messages? | Optional, in the same macro: `VX_ASSERT(Index < Num)` or `VX_ASSERT(Index < Num, "Index {} is too big", Index)`. |
| Macro prefix? | `VX_` for every Vertex macro. Reflection's markers become `VX_CLASS()` and `VX_PROPERTY()`. |
| `VX_CHECK` failing many times? | It reports only the first failure at each line, so per-frame code can't flood the log. |
| How do we test it? | Every failure goes through one replaceable function, the **handler**. CoreTests swaps in a handler that throws, so doctest can catch the failure instead of the test run ending. |
| Global variables? | Only inside one `.cpp`, reached through functions (`SetAssertHandler`). Now in CLAUDE.md's naming table. |

### How each macro behaves

| Macro | When false (Debug, Development) | In Shipping |
|---|---|---|
| `VX_ASSERT(X)` | Logs, then ends the program | Removed. `X` doesn't run. |
| `VX_VERIFY(X)` | Logs, then ends the program | `X` still runs; the result isn't checked |
| `VX_CHECK(X)` | Logs once, keeps going, returns `false` | `X` still runs and returns its result |

## Smaller choices Claude made (change any)

- **Logging v2 replaces v1.** There will be one `Log`, which always takes a format, like C++23's `std::print`. Your four existing `Log` calls compile unchanged. To log a string held in a variable, write `Log(ELogLevel::Info, "{}", Text)`.
- **A wrong message fails the build.** `VX_ASSERT(Ok, "{} of {}", Index)` is missing an argument, and the compiler says so, even in Shipping.
- **No heap memory when something fails.** Messages are written into a fixed 1024-character buffer on the stack and cut off there. When an assert fires, memory itself might be what's broken, so the failure path shouldn't depend on it. UE does the same.
- **A passing assert costs one comparison.** The message is only built after a failure. `VX_ASSERT` also avoids lambdas, because in Debug every lambda is a real function call, and `TArray` will check every index.
- **One definition per macro for all configurations.** Shipping's version comes from the same line of code (`VERTEX_ENABLE_ASSERTS && …`), not a second copy that can drift.
- **The failure line is clickable in Visual Studio:**
  `[Error] VX_ASSERT failed: Index < Num, at C:\…\Array.h(120). Index 7 is out of range`
- **Where it lives:** `Core/Debug/Assert.h`. UE's is `Misc/AssertionMacros.h`.
- **In CoreTests, the throwing handler is installed for every test.** So an assert that fires by surprise inside Core code fails only that test, with a readable message, and the other tests still run.

## How a failure travels

```
VX_ASSERT(Index < Num, "Index {}", Index)        in your code
  └─ false → FailAssert(...)                     Assert.h: checks the message at compile time
              └─ HandleAssertFailure(...)        Assert.cpp: the one place every failure goes
                   ├─ format the message         into the 1024-character stack buffer
                   ├─ call the handler           default: write the Error line with Log
                   └─ end the program            (CoreTests' handler throws before this)
```

## UE references (study, never copy)

In `C:\Developer Projects\HNDREDGAMES\UE_5_7_4\Engine\Source\Runtime\Core\`:
- `Public/Misc/AssertionMacros.h:224`: `check` and `verify`. Line 312 is the separate Shipping copy that Vertex avoids.
- `Public/Misc/AssertionMacros.h:234`: Epic's note on why `check` avoids lambdas.
- `Public/Misc/AssertionMacros.h:340`: how `ensure` reports once and keeps going.
- `Private/Misc/AssertionMacros.cpp:697`: the one function every failed `check` goes through, like Vertex's `HandleAssertFailure`.

## Files

| File | What changes | Who |
|---|---|---|
| `Core/Private/Logging/FormatToBuffer.h` + `.cpp` | New: formats text into a fixed buffer | Fares |
| `Core/Public/Core/Logging/Log.h` + `Private/Logging/Log.cpp` | Logging v2 | Fares |
| `Core/Public/Core/Debug/Assert.h` + `Private/Debug/Assert.cpp` | New: the failure path, the handler, the macros | Fares |
| `Tests/CoreTests/Private/Debug/AssertRecorder.h` + `.cpp` | New: the throwing test handler | Fares |
| `Tests/CoreTests/Private/CoreTestsMain.cpp` | Installs the test handler | Fares |
| `Tests/CoreTests/Private/Debug/AssertTests.cpp` | New: 8 tests | Fares |
| Comments in every header | After each card | Claude |
| CLAUDE.md, the review skill, README | `VX_` prefix and globals rule (done); a new § Asserts at the end | Claude |

All paths are under `Source/`. Create files on disk, then run `Scripts\Setup.bat`.

## Cards

Each card gets a full walkthrough when we reach it. Here's the map.

**Card 1: format into a fixed buffer.** One function, `FormatToBuffer`, takes a buffer, a format, and arguments, and returns the text it wrote. It stops writing when the buffer is full.
*Why first:* both Logging v2 and the asserts use it.

**Card 2: Logging v2.** `Log` becomes a template that checks the format at compile time and hands everything to one regular function in `Log.cpp`.
*Why split it that way:* the template part is tiny, so every call site stays cheap to compile. The real work happens once, in the `.cpp`.

**Card 3: the failure path.** Assert.h gets the failure info struct (`VAssertFailure`), `SetAssertHandler`, and `HandleAssertFailure`. Assert.cpp formats the message, calls the handler, then ends the program. The default handler writes the Error line.
*Why:* every failure goes through one place, so there's one place to change what a failure does. Later that could be an error window in the editor, or a crash reporter.

**Card 4: the three macros.** `VX_ASSERT`, `VX_VERIFY`, `VX_CHECK`. Each one captures the expression as text, plus the file and line, and calls the failure path only when the expression is false.
*Why last of the Core cards:* a macro is the thin front door. Everything it calls already exists by then.

**Card 5: the test handler.** CoreTests gets a handler that records `VX_CHECK` failures and throws for the other two. `main` installs it before any test runs.

**Card 6: the tests.** In all three configurations, they prove:
1. `VX_ASSERT` stays silent when the expression is true.
2. A failing `VX_ASSERT` reports the right expression, line, and message.
3. `VX_ASSERT` never runs its expression in Shipping.
4. `VX_VERIFY` runs its expression in every configuration.
5. A failing `VX_VERIFY` stops like `VX_ASSERT`, except in Shipping.
6. `VX_CHECK` returns the expression's result and keeps going.
7. `VX_CHECK` reports only the first failure at each line.
8. A message longer than the buffer is cut, not overflowed.

**Card 7: see it for real, then undo it.** Put a failing `VX_ASSERT` in the Editor, run it in all three configurations, and run it once under the Visual Studio debugger. Then add a wrong message and watch the build fail. None of this is committed.

## How we know it works

- The whole solution builds with zero warnings in Debug, Development, and Shipping.
- `VertexCoreTests.exe` passes 10/10 tests (the 2 existing ones plus these 8) in all three.
- Card 7: in Debug and Development the Editor prints the Error line and ends with a non-zero exit code. In Shipping it runs normally.

Before writing this plan, Claude tested every risky part in a scratch folder with Vertex's exact compiler settings. That covered the three macros in Debug and Shipping, the fixed buffer, the throwing test handler with doctest, and a real termination, which showed no pop-up dialog and exit code 3.

## After the cards

1. **Docs (Claude):**
   - A new CLAUDE.md § Asserts: which macro to use when, and never put work that must happen inside `VX_ASSERT`.
   - Updates to the review skill and README.
2. **Review:** `/review` on your word, until "Review passed".
3. **PR:** when you say so. Merged after you've read the diff.
4. **Next feature:** Memory and `TUniquePtr`.

## Later (not in this feature)

- **Stop in the debugger at the exact failing line**, before ending the program. That needs an OS call ("is a debugger attached?"), and Core has no OS code yet. Revisit with M2's platform layer, or sooner if card 7 shows the debugger stopping somewhere unhelpful.
- **A call stack in the Error line.** Revisit when an assert is hard to trace from file and line alone.
- **`VX_CHECK_ALWAYS`**, which reports every failure, not just the first. Added when first needed.
- **raylib's log through `Vertex::Log`:** M2, with the first window.
