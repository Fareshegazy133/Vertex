---
name: roadmap
description: Show Vertex's roadmap (milestones, features, status, what's next) and discuss changes of direction with Fares. Use when Fares asks where the project stands or what's next, or wants to add, drop, reorder, or break down milestones and features.
argument-hint: "[M2 | a topic to discuss]"
allowed-tools: Read Grep Glob Bash(git status *) Bash(git branch *) Bash(git log *) Bash(cat "${CLAUDE_PROJECT_DIR}/.claude/plans/Roadmap.md")
---

# Vertex roadmap

`.claude/plans/Roadmap.md` is the single source of truth for what Vertex builds, in what order, and where each feature stands. This skill shows it and changes it. The roadmap's "How to read it" section defines milestones, IDs, and statuses. Follow it.

Arguments: `$ARGUMENTS`

## Context

Branch and working tree:
!`git status --short --branch`

Work branches:
!`git branch --list "feature/*" "fix/*" "chore/*" "docs/*"`

The roadmap on this branch:
!`cat "${CLAUDE_PROJECT_DIR}/.claude/plans/Roadmap.md"`

## 1. Show where the project stands

**Without arguments**, show this overview, then stop and ask what Fares wants to discuss. Don't paste the whole file. It's long, and the overview is what he reads.

```
# Vertex roadmap · <today's date>

**Now:** M1.2 Memory and TUniquePtr · Todo · run /plan-feature to start it
**Branch:** master, clean

| Milestone | Progress |
|---|---|
| M0 The skeleton builds | Done, tagged m0 |
| M1 Core types | 2 of 7 done |
| M2 The application and its window | Draft, 5 features |
| M3 Reflection (HeaderTool v0) | Not broken down |

## M1: Core types
<the current milestone's table: ID, Feature, Status>

## Up next
<the next milestone's table, or its goal if it isn't broken down>

**Triggers to watch:** <Unscheduled items or Tooling ideas whose trigger the current or next feature fires. Omit the line if there are none.>
```

- **Now** is the feature In progress, if there is one. Otherwise it's the first Planning or Planned feature, and otherwise the first Todo feature in build order. Name the skill that continues it: `/implement-feature` for In progress and Planned, `/plan-feature` for Planning and Todo.
- **Progress** counts Done features out of every feature that isn't Dropped.
- **A newer roadmap elsewhere:** if a `feature/…` branch exists for a feature this roadmap still calls Todo, the roadmap on that branch is newer. Say so.

**With an argument:**
- A milestone (`M2`, `m2`, `2`): show that milestone in full (goal, table, notes), then discuss it.
- Anything else is a topic. Show the overview's first two lines, then go straight into the topic.

## 2. Discuss

Fares decides the direction. You bring the principal engineer's view (user CLAUDE.md, Principal Engineer Mandate):

- Lead every message with the ask, under "What I need from you", answerable in a word. Short, plain sentences, and one question per message.
- For each fork, give two or three options in a table, say which one a principal would pick, and why. Then he chooses.
- Push back when a change:
  - **breaks the dependency order.** A feature may use only what's above it. Name the feature it would need first.
  - **breaks a CLAUDE.md rule.** Name the rule, why it exists, and what changing it would cost.
  - **pulls in work with no trigger yet** ("we might need it"). The Unscheduled table is where that waits.
- Name the UE counterpart when it helps: what problem Epic solved, and whether Vertex has that problem yet. UE 5.7.4's source is at `C:\Developer Projects\HNDREDGAMES\UE_5_7_4`. Study it; never copy it.

### Breaking a milestone down

A milestone that says "Not broken down into features yet", or is marked Draft, needs this before its first feature is planned:

1. Read the milestones before it, the plans of their features, and the UE counterparts in its notes.
2. Propose features in dependency order. Each one is one branch and one PR that leaves `master` green: small enough to plan and review in one conversation, big enough to prove something on its own. Give each a one-line "what it proves".
3. Write the notes each feature's planning will need: its UE counterpart, its forks, and any Unscheduled item whose trigger it fires.
4. Fares picks and edits. Remove the Draft line only when he says the list is settled.

## 3. Change the roadmap

Before editing, show the change: the rows as they are and as they will be, plus any notes added or removed. Edit only after Fares says yes.

- **IDs never change and are never reused.** A new feature takes its milestone's next free number: the highest ID it has ever used, Dropped rows included, plus one. Its row goes where it will be built.
- **Reordering moves rows.** It never renumbers.
- **Dropping keeps the row**, with status Dropped and the reason in the feature's notes.
- **A new milestone** takes the next free milestone number and goes where it will be built, like a feature.
- **A Done row never changes.** Its notes may gain a line that points at a later change.
- **A Planning, Planned, or In progress feature:** changing its scope changes its plan too. Say so, and point at `/plan-feature <ID>` to revise the plan.
- **A trigger that fires** moves its item out of Unscheduled or Tooling ideas and into a milestone, with an ID.
- **A change that needs CLAUDE.md to change** (a module, a rule) changes it in the same commit. Name the section.
- This skill sets only the Todo and Dropped statuses. The other skills set the rest (the roadmap's status table says which).

## 4. Record the change

Roadmap changes go through git like code, so history shows when the direction changed, and why.

- **On `master` with a clean tree:** cut `docs/roadmap-<topic>` from the latest `master`, commit, push, raise a PR, and merge it. `docs/` PRs are Claude's to merge (CLAUDE.md § Git workflow), and Fares approved the content in step 3. Merge with `gh pr merge <N> --squash --subject "<title> (#<N>)" --body-file <file>`, then return to `master` and pull.
- **On a work branch:** commit the roadmap change on that branch, in its own commit. Tell Fares it reaches `master` with that branch's PR, and that the PR body will list it.
- **On `master` with uncommitted changes:** ask Fares before touching git.

Mechanics:
- Commit messages follow CLAUDE.md § Git workflow: an imperative summary of at most 72 characters, a blank line, then why. End them, and PR bodies, with the attribution lines the session's instructions give.
- Write message and body files to the scratchpad with the Write tool, then `git commit -F <file>` and `gh pr create … --body-file <file>`. PowerShell 5.1's `Out-File -Encoding utf8` adds a BOM, which shows up in the PR.
- `gh` works from PowerShell directly. From Git Bash, prefix `MSYS_NO_PATHCONV=1` and pass files as `$(cygpath -w <file>)`.

## 5. Close

End with what changed (the IDs), what's next ("Now: …"), and the skill that starts it.
