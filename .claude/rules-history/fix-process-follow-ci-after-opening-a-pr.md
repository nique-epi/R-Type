# History: follow the CI after opening a PR

## Context
A draft PR was opened and the answer ended with the local results and "the CI covers GCC and MSVC". The CI was not read afterwards.

## Mistake
The PR was reported as delivered without looking at the state of its checks.

## Root cause
No rule required reading the CI after `gh pr create`; the local build was treated as enough.

## Rule
See `.claude/rules/fix/process/fix-process-follow-ci-after-opening-a-pr.md`.

## Update (24 October 2026) — A pull request is built merged with its base

### Context

Pull request #103 (visual effects) answered a review with three commits that restructured the tests: among others, the fixture `ServerConnectionTest` became `ServerConnectionFixture`. Meanwhile `main` gained #105, which adds two tests written `TEST_F(ServerConnectionTest, …)`. My own notes said "`main` moved 2 commits, a conflict is expected".

### Mistake

The three commits were built and tested alone on the branch, then pushed. GitHub kept the PR "mergeable", since git saw no conflicting lines, but its CI builds `refs/pull/103/merge`: Linux, Windows and the clang-tidy job holding `ServerConnectionTest.cpp` failed on the unknown fixture. The user found the PR red.

### Root cause

The checks before the push ran on the branch only; the base that had moved was not merged in, although the conflict had been announced.

### Rule

Compare the merge base with the current base before every push to a PR; when the base moved, merge it in a detached worktree, search the merged tree for every name the branch renamed, build and test the merge, and push only once it is green.
