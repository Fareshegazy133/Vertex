---
name: ship-feature
description: Ship a reviewed Vertex feature (CLAUDE.md Feature workflow, steps 5–6). Verifies the build, commits, raises the PR, merges on Fares' go, marks the feature Done in the roadmap, and hands off to the next conversation. Only Fares starts it.
argument-hint: "[M1.2]"
disable-model-invocation: true
allowed-tools: Read Grep Glob Bash(git status *) Bash(git branch *) Bash(git log *) Bash(git diff *) Bash(git -c core.safecrlf=false diff *) Bash(cat "${CLAUDE_PROJECT_DIR}/.claude/plans/Roadmap.md")
---

# Ship a Vertex feature

Steps 5 and 6 of CLAUDE.md § Feature workflow. Running this skill is Fares' go to commit and raise the PR. Merging an engine feature needs a second go, after he has read the diff.

Arguments: `$ARGUMENTS`

## Context

Branch and working tree:
!`git status --short --branch`

Commits on this branch:
!`git log --oneline master..HEAD`

Files changed against `master`:
!`git -c core.safecrlf=false diff --stat master`

The roadmap on this branch:
!`cat "${CLAUDE_PROJECT_DIR}/.claude/plans/Roadmap.md"`

## 1. Find the feature

**With an argument:** its ID (`M1.2`, `m1.2`, `1.2`) or its name. **Without one:** the feature In progress. It must be checked out on its own branch. Never ship from `master`.

Stop and tell Fares if:
- **the feature isn't In progress.** Todo, Planning, or Planned means it isn't implemented. Done means it's already shipped.
- **a card is unticked** in the plan. Name it; `/implement-feature` finishes it.
- **the review hasn't passed.** If this conversation doesn't hold "Review passed." for this branch, or the code changed after it, say so and offer `/review branch --build`. Never skip it silently.

## 2. Verify from clean

`master` is always green (CLAUDE.md § Git workflow), so check the branch the way `master` will see it:
1. Run `git fetch`. If `origin/master` moved since the branch was cut (`git log --oneline HEAD..origin/master`), merge `origin/master` into the branch. On a roadmap conflict, keep both sides' changes. Never rebase a branch that's already pushed.
2. Run `Scripts\Setup.bat`, then build the whole solution in Debug, Development, and Shipping with zero warnings (CLAUDE.md § Commands).
3. Run the tests in all three configurations.

Any failure stops the ship. Report its first error and go back to fixing it with Fares.

## 3. Finish the records

Do this before committing, so the PR carries it:
- **The plan:** status "done <date>"; every card ticked; and, if the review added work, an "Added during review" section above "Later" that says what and why.
- **The roadmap:** the feature's row becomes Done. Every item in the plan's "Later" section moves into the roadmap: into the notes of the feature it belongs to, or into the Unscheduled table with its trigger. The roadmap is the backlog. A deferral that lives only in an old plan gets lost.
- **Docs:** CLAUDE.md and README say what the feature changed. The review's doc-staleness check should already have caught this. If this is the milestone's last feature, README's Status line moves on to the next milestone.

## 4. Commit and raise the PR

1. **Stage by path.** Run `git status` and stage the feature's files. Nothing generated is staged (`Binaries/`, `Intermediate/`, `Vertex.sln`, `*.vcxproj`, `ThirdParty/*/Source/`). Ask about any file you can't account for.
2. **Commit.** The message follows CLAUDE.md § Git workflow: an imperative summary of at most 72 characters ("Add TUniquePtr and MakeUnique"), a blank line, then why. Write it to a scratchpad file with the Write tool, then `git commit -F <file>`. End it with the attribution lines the session's instructions give.
3. **Push:** `git push -u origin <branch>`.
4. **Raise the PR** into `master`. The title is the commit summary. Write the body to a scratchpad file with the Write tool:

   ```markdown
   ## Summary
   <what the feature adds, and the problem it solves, in two or three sentences>

   Roadmap: M1.2. Plan: `.claude/plans/m1-unique-ptr.md`.

   ## Decisions
   - <the key decisions, one line each>

   ## Verification
   - Builds with zero warnings in Debug, Development, and Shipping.
   - `VertexCoreTests.exe` passes <n>/<n> in all three.
   - <the plan's manual checks>

   ## Review
   "Review passed." Skipped INFO findings: <the list, or "none">.

   ## Also in this PR
   <roadmap changes made on this branch, other docs. Omit the section if there are none.>

   <the attribution line the session's instructions give for PRs>
   ```

   Then run `gh pr create --base master --head <branch> --title "<title>" --body-file <file>`.
5. **Record the PR number** in the roadmap row's PR column and in the plan's status line ("done <date>, PR #<N>"). Commit that (`Record PR #<N> for M1.2`) and push.

## 5. Wait for Fares

Give him the PR URL and the diff summary, then stop.
- **A `feature/` or `fix/` PR** holds his engine code. He reads the diff first (`gh pr diff <N>`, or the PR's Files tab), and you merge only when he says go.
- **A `chore/` or `docs/` PR** holds Claude-owned work. Merge once it's verified (CLAUDE.md § Git workflow), unless he asked to read it.

## 6. Merge

1. Run `gh pr merge <N> --squash --subject "<title> (#<N>)" --body-file <file>`. The body is the commit message's why, plus the attribution lines. The repo is squash-only and deletes the remote branch itself.
2. Confirm the merge: `gh pr view <N> --json state` says `MERGED`.
3. Run `git switch master` and `git pull --ff-only`. Delete the local branch with `git branch -D <branch>`. Git doesn't count a squash-merged branch as merged, so `-d` refuses; the `MERGED` state is what makes `-D` safe. Then `git fetch --prune`.
4. `master`'s tree should now match the PR's last commit (`git diff <that commit> master --stat` is empty). If it doesn't, something else merged in between: build and test `master` once more.

## 7. Milestone tag

If this was the milestone's last feature that isn't Dropped, offer to tag it, and tag only on Fares' yes. Tags are annotated, like `m0`: run `git tag -a m1 -m "M1: <the milestone's goal, shortened>"`, then `git push origin m1`.

## 8. Hand off

1. **Memory.** Update the project-status memory (the "START HERE" entry in MEMORY.md) with what merged (the PR and `master`'s SHA) and the next feature's ID. Add only what the next conversation needs that the repo doesn't hold, such as a preference Fares stated or a half-finished discussion about the next feature. Order, status, forks, and deferrals live in the roadmap, so don't copy them.
2. **Tell Fares**, in this order:
   - what shipped, with the PR link;
   - what's next: its ID, its name, and one line on what it does;
   - "Run `/plan-feature` to start it.";
   - that it's safe to clear the chat.

## Mechanics

- `gh` works from PowerShell directly. From Git Bash, prefix `MSYS_NO_PATHCONV=1` and pass files as `$(cygpath -w <file>)`.
- Write commit messages and PR bodies with the Write tool. PowerShell 5.1's `Out-File -Encoding utf8` adds a BOM, which shows up in the PR.
- Read exit codes in PowerShell with `$LASTEXITCODE`.
- Never force-push `master`, and never skip hooks.
