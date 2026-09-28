## What
<!-- One or two sentences: what does this change? -->

## Why
<!-- The reason, and the milestone step it belongs to (e.g. "M0 step 7"). -->

## How it was tested
- [ ] `Scripts\Setup.bat` succeeds
- [ ] Builds in Debug, Development, and Shipping with zero warnings
- [ ] Ran it: <!-- what you ran and what you saw -->

## Rules check (see CLAUDE.md)
- [ ] No `raylib.h` / `rlgl.h` / `raymath.h` outside `Source/Runtime/Private/Platform/Raylib/`
- [ ] No raylib types in any `Public/` header
- [ ] No "upward" includes between Runtime folders
