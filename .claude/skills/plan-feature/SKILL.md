---
name: plan-feature
description: Plan one Vertex feature with Fares (CLAUDE.md Feature workflow, steps 1–2). Finds the feature in the roadmap, explains the problem it solves, raises the design forks one at a time, then cuts the branch and writes the plan. Use when Fares wants to plan a feature, or the next one. Usage - /plan-feature [M1.2 | name]
argument-hint: "[M1.2 | m1.2 | 1.2 | TArray]"
allowed-tools: Read Grep Glob Bash(git status *) Bash(git branch *) Bash(git log *) Bash(cat "${CLAUDE_PROJECT_DIR}/.claude/plans/Roadmap.md")
---

# Plan a Vertex feature

Steps 1 and 2 of CLAUDE.md § Feature workflow: plan together, then write the plan. Fares makes every design call; you recommend. Nothing gets implemented here. That's `/implement-feature`, after he approves the plan.

Arguments: `$ARGUMENTS`

## Context

Branch and working tree:
!`git status --short --branch`

Work branches:
!`git branch --list "feature/*" "fix/*" "chore/*" "docs/*"`

The roadmap on this branch:
!`cat "${CLAUDE_PROJECT_DIR}/.claude/plans/Roadmap.md"`

## 1. Find the feature

**With an argument:**
- An ID: `M1.2`, `m1.2`, or `1.2`.
- Otherwise, a name. Match it against the roadmap's feature names, ignoring case and backticks (`TArray`, `unique ptr`). If several match, ask which one. If none match, say so and offer to add it with `/roadmap`. Never add a feature here.

**Without an argument:** the first Planning feature in build order (a plan already under way), otherwise the first Todo feature. If the next milestone isn't broken down into features yet, or is marked Draft, say so and offer `/roadmap` to settle it first.

| Found | Do |
|---|---|
| Todo | Plan it. |
| Planning | Resume. Read its plan file and continue the discussion where it stopped. |
| Planned or In progress | Its plan is already approved. Ask: revise the plan (that reopens approval and sets the status back to Planning), or continue with `/implement-feature`? |
| Done | Say so, with its PR. Offer the next Todo feature. |
| Dropped | Say so, with the reason from its notes. |

Then check these, and stop to ask Fares if one fails:
- **Dependency order.** If a feature above it isn't Done, name that feature and say what this one would build on before it exists. Recommend finishing it first; Fares decides.
- **The working tree.** Planning cuts a new branch from `master`. Uncommitted changes, or another work branch checked out, need Fares' call first. Never carry changes onto the new branch silently.
- **A newer roadmap elsewhere.** If a `feature/…` branch exists for a feature this roadmap calls Todo, the roadmap on that branch is newer. Say so before going on.

## 2. Prepare, before the first message

Read everything the plan will depend on, so the first message is already informed:
- **The feature's notes in the roadmap:** its UE counterpart, its forks, and lessons from the old code.
- **The plans of the Done features it builds on.** Their "Later" sections may name this feature as a trigger.
- **The code it will touch or call, as it is now.** The plan describes the code on disk, not the code as memory remembers it.
- **The UE counterpart** in the local 5.7.4 source (`C:\Developer Projects\HNDREDGAMES\UE_5_7_4\Engine\Source\`). Note a `file:line` for each part worth discussing: what problem Epic solved, and whether Vertex has it yet. Study, never copy (CLAUDE.md § Unreal Engine reference).
- **CLAUDE.md**, for every rule the feature touches, and its "Decided when first needed" list.
- **Prototypes.** Prove every risky claim the plan will rest on (a compiler behavior, a warning, how an API behaves) in the scratchpad, with Vertex's flags: `/std:c++20 /W4 /WX /permissive- /Zc:preprocessor /EHsc`. Try Debug and Shipping settings where it matters. A plan that promises something the compiler rejects costs Fares a card.

## 3. Plan together (workflow step 1)

**The problem first**, in one short message:
- What's missing or painful today, concretely. Which upcoming feature needs this one, and what that feature would have to do without it.
- How this feature solves it: what will exist when it's done.
- The UE counterpart, and the problem Epic solved with it.
- The map: the forks to decide, numbered, one line each.

**Then one fork per message:**
- Start with "What I need from you": the question, answerable in a word.
- Give two or three options in a table, say which one a principal would pick, and why: the tradeoff, and what the rejected option would cost later.
- Wait for his choice before the next fork. Answer follow-up questions fully before moving on.
- Teach the one concept the fork needs, at the depth it needs. He knows C++, so spend words on architecture, the build and link model, performance, and API design.

**Smaller choices** have a clear default, so they aren't forks. Make them, and list them in the plan under "Smaller choices Claude made", where he can change any of them.

**Push back** when a choice breaks a CLAUDE.md rule: name the rule and why it exists, give the better path, and let him make the call.

**Conventions.** If the feature needs something from CLAUDE.md's "Decided when first needed" list, or anything the naming table doesn't cover, that's a fork. Once he chooses, the plan's docs step records it in CLAUDE.md, and in the review skill and StyleScan if it can be checked.

## 4. Write the plan (workflow step 2)

When every fork is decided:
1. **Cut the branch** from the latest `master`: `git switch master`, `git pull --ff-only`, then `git switch -c <branch>`.
   - Engine work: `feature/<milestone>-<topic>`, for example `feature/m1-unique-ptr`.
   - A chore feature (Claude-owned work: build files, scripts, tooling, docs): `chore/<topic>`.
   - `<topic>` is short kebab-case. It names the plan file too.
2. **Write** `.claude/plans/<milestone>-<topic>.md` from the template below.
3. **Update the roadmap:** status Planning, and the plan linked in the Plan column (`[m1-unique-ptr](m1-unique-ptr.md)`).
4. **Paste the full plan** into the reply. Fares reads plans in the terminal; the file is the record.
5. **Ask for his approval.**

For a change, edit the file, then show only the changed sections, and name them.

## 5. On approval

1. The plan's status line becomes "approved <date>", and the roadmap row becomes Planned.
2. Commit the plan and the roadmap change on the feature branch, for example `Plan M1.2: Memory and TUniquePtr`, with the why in the body. Write the message to a scratchpad file with the Write tool and `git commit -F <file>`, ending it with the attribution lines the session's instructions give. CLAUDE.md § Feature workflow keeps the plan on the branch, so the PR carries the design. Don't push yet: `/ship-feature` pushes.
3. Ask whether to start card 1 now. When he says yes, invoke the `implement-feature` skill with the feature's ID.

## Plan template

Plain enough to read in one pass: decisions, files, and cards as "what and why". Exact syntax belongs in each card, when it's presented, not in the plan.

For a chore feature, the plan is shorter: the problem, the decisions, the files, and the cards. Its cards say Who: Claude, and `/implement-feature` writes them and walks Fares through each.

````markdown
# M1.2: Memory and `TUniquePtr`

**Status:** planning
**Branch:** `feature/m1-unique-ptr`
**Roadmap:** M1, Core types

## The problem

<What's missing or what hurts today, concretely. Which later feature needs this one, and what it would have to do without it.>

## How this feature solves it

When this feature is done:
- <what exists>
- <what it makes possible>

## Decisions (Fares, <date>)

| Question | Decision | Why |
|---|---|---|

## Smaller choices Claude made (change any)

- **<choice>.** <one or two sentences of why>

## How it fits

<Where it sits in the module graph and the Runtime layers, what calls it, and what it calls. A small text diagram when the flow has several steps.>

## UE references (study, never copy)

- `<path>:<line>`: <what to look at, and what Vertex takes or leaves>

## Files

| File | What changes | Who |
|---|---|---|

All paths are under `Source/`. Create new files on disk, then run `Scripts\Setup.bat`.

## Cards

Each card gets a full walkthrough when we reach it. `/implement-feature` ticks a card once it builds and passes review.

- [ ] **Card 1: <title>.** <what it builds, in one or two sentences>
  *Why here:* <why it comes at this point in the order>
- [ ] **Card 2: <title>.** …

## Tests

In all three configurations, they prove:
1. <one behavior per line, the way its test case name would say it>

## How we know it works

- The whole solution builds with zero warnings in Debug, Development, and Shipping.
- `VertexCoreTests.exe` passes <n>/<n> in all three.
- <any manual check: run the Editor, or a negative test that must fail to compile>

## After the cards

1. **Docs (Claude):** <CLAUDE.md sections, README, the review skill, StyleScan>
2. **Review:** `/review`, until "Review passed".
3. **Ship:** `/ship-feature`, when Fares says so.

## Later (not in this feature)

- **<item>.** Trigger: <what makes it necessary>.
````

Sections added while working go just above "Later": "Added during implementation" and "Added during review", each with its date.

## Rules

- Lead every message with the ask. Short, plain sentences, one idea per bullet, and one new concept per message.
- Never write engine C++ here, or anything a card will ask Fares to write. A short snippet may illustrate a fork's options.
- Cite CLAUDE.md by section; don't restate it.
- Every item the plan defers names its trigger. `/ship-feature` moves those items into the roadmap.
