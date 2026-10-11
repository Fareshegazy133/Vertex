---
name: implement-feature
description: Walk Fares through implementing a planned Vertex feature, one card at a time (CLAUDE.md Feature workflow, step 3). Resumes at the first unticked card in the plan, so it works in a fresh chat. Use when Fares wants to start or continue implementing a feature. Usage - /implement-feature [M1.2 | name]
argument-hint: "[M1.2 | m1.2 | 1.2 | TArray]"
allowed-tools: Read Grep Glob Bash(git status *) Bash(git branch *) Bash(git log *) Bash(git diff *) Bash(git -c core.safecrlf=false diff *) Bash(cat "${CLAUDE_PROJECT_DIR}/.claude/plans/Roadmap.md") Bash(bash "${CLAUDE_PROJECT_DIR}/.claude/skills/review/Scripts/StyleScan.sh" *)
---

# Implement a Vertex feature

Step 3 of CLAUDE.md § Feature workflow. The plan is the map; this is the guided walk. Fares writes the engine C++, one card at a time. You present each card, check what he wrote, and do the chores.

Arguments: `$ARGUMENTS`

## Context

Branch and working tree:
!`git status --short --branch`

Work branches:
!`git branch --list "feature/*" "fix/*" "chore/*" "docs/*"`

The roadmap on this branch:
!`cat "${CLAUDE_PROJECT_DIR}/.claude/plans/Roadmap.md"`

## 1. Find the feature

**With an argument:** an ID (`M1.2`, `m1.2`, `1.2`), or a name matched against the roadmap's feature names, ignoring case and backticks.

**Without an argument:** the feature In progress, otherwise the first Planned feature in build order. It's **not** the next Todo feature: that one has no approved plan, and CLAUDE.md § Feature workflow says nothing is implemented before Fares approves one.

| Found | Do |
|---|---|
| In progress | Continue at the first unticked card. |
| Planned | Start at card 1, and set the roadmap row to In progress. |
| Todo or Planning | There's no approved plan yet. Say so, and offer `/plan-feature <ID>`. |
| Done | Say so, with its PR. |
| Nothing In progress or Planned | Say so, name the next Todo feature, and offer `/plan-feature`. |

If a `feature/…` branch exists for a feature this roadmap calls Todo or Planning, the roadmap on that branch is newer. Switch to it (step 2) and read again.

## 2. Get ready

1. **The branch.** The plan names it. If another branch is checked out, switch when the tree is clean. Otherwise, ask Fares first.
2. **Read, in full:** the plan, the CLAUDE.md sections it touches, and every file the remaining cards change, as it is now. Fares sometimes edits files in parallel, so never trust an earlier read.
3. **Find the next card:** the first `- [ ]` under "Cards". If every card is ticked, go to step 5.
4. **When resuming**, open with one line: the feature, how many cards are done, and which card is next. Then present it.

## 3. Present one card

The card format is Mentorship Mode rule 7 in the user CLAUDE.md, without its Check-in line. Fares dropped check-ins on 2026-10-05.

```
## Card <n> of <total>: <title>

**File:** `Source/…` (new | edit)
**What:** <one line: the unit being built>
**Why:** <two or three sentences: the reasoning, the plan's decision behind it, and how it fits the wider system>
**UE reference:** `<path>:<line>`, <what to notice>. Omit the line when there's no counterpart.

**Write it:**
1. <a micro-step small enough to type and check, with the exact syntax>
2. …

**Verify:** `<exact command>`. Expect: <the output>.
```

- **Verify every syntax and API claim before presenting it.** When you aren't sure, prototype in the scratchpad with Vertex's flags (`/std:c++20 /W4 /WX /permissive- /Zc:preprocessor /EHsc`). A card that doesn't compile as written costs him a round trip.
- **Open the UE reference you cite**, and point at the right line.
- **Keep micro-steps terse.** He knows C++. Spend the words on the build and link model, MSVC `/W4` traps, performance, API design, and architecture.
- **One new concept per card.** If a card needs two, split it, or say "there's one more idea in here; we'll reach it at step 4".
- **Name the trap before it bites**, the first time it's relevant: a warning that only fails in Shipping, an `.obj` dropped from a static library, a parameter that shadows a member.
- **Comments are yours.** Tell him to skip doc comments: you add them after the card (CLAUDE.md § Comments).
- **New files** are created on disk under `Source/<Module>/`, never through the IDE, so the card's Verify starts with `Scripts\Setup.bat`.
- **No predict-first or check-in questions.** Verify states the expected output instead.
- **A chore card** (Who: Claude, in the plan's Files table) is yours: write it, verify it, then walk him through what it does and why.

Then wait for him.

## 4. When he says a card is done

1. **Read his files again**, every one the card touched.
2. **Fix these silently.** Never mention them in the review:
   - A UTF-8 BOM. His editor adds one to every new file. Strip it with `sed -i '1s/^\xEF\xBB\xBF//' <file>`.
   - Include order: put includes in CLAUDE.md § File layout order.
   - Indentation: snippets pasted from the terminal can arrive with spaces instead of tabs.
3. **Add or refresh the comments** for every declaration the card touched. Re-read CLAUDE.md § Comments first: its tag tables are the source of truth.
4. **Run StyleScan** on the touched files: `bash "${CLAUDE_PROJECT_DIR}/.claude/skills/review/Scripts/StyleScan.sh" <paths>`. Fix the findings that are yours (comments, encoding, includes). Keep his for the review.
5. **Build, and run the card's Verify yourself**, in Debug (CLAUDE.md § Commands). If the card touches configuration-sensitive code (an `#if VERTEX_*` block, an assert), build Shipping too.
6. **Review, kindly and briefly.** What's good, specifically and why. Then what to change, ordered correctness → design → style, each with the principle behind it. Report, don't fix: his engine C++ is his to change. Match the depth to the system: a few lines for utilities, full depth for architecture.
7. **When the card is clean**, tick it in the plan (`- [x]`) and present the next card in the same reply.

**When something breaks**, diagnose with him before giving the answer. Point at the first error, not the last; say what that kind of error means, and where to look. Give the fix once he has tried, or when he asks, then say how you found it.

**When he says "write it"**, write it. Then walk through what you wrote, and leave him a small adjacent piece where there is one. He often asks you to write the tests: the same rule applies.

**When the work changes a decision in the plan**, update the plan in the same step (the Decisions table, or an "Added during implementation" section above "Later") and say what changed. The plan stays true to the code.

## 5. After the last card

1. Do the docs chores in the plan's "After the cards".
2. Run `Scripts\Setup.bat` if files were added. Build the whole solution in Debug, Development, and Shipping with zero warnings, and run `VertexCoreTests.exe` in all three.
3. Tell Fares the feature is ready for `/review`. The roadmap row stays In progress until `/ship-feature` merges it.

## Rules

- Lead every message with the ask. Short, plain sentences, and one idea per bullet.
- Don't commit during implementation unless Fares asks. `/ship-feature` commits after the review.
- Cite CLAUDE.md by section; don't restate it.
