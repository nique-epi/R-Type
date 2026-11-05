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

## Update (5 November 2026) — A conflict resolved toward the side that moved the code

### Context

Pull request #227 (entity type sheets) merged `main` after #219, which made the connection module of the client provide `IServerTicks`, the clock of the server, with one more `context.provide` line in `ClientConnectionModule::install()`. The branch had moved every `provide` of that function into `PlayerConnection::provideParts()`.

### Mistake

The hunk of `ClientConnectionModule.cpp` opposed the branch's call to `provideParts()` and main's list of `provide` lines; it was resolved by keeping the branch side. The merge built, 3991 tests passed, and nothing provided `IServerTicks` any more, while the screens require it when they are installed: the client would have stopped at its start. The loss was only seen because clang-tidy reported the include of `IServerTicks.hpp` as unused.

### Root cause

The resolution compared the two sides as a choice between them, and never listed what the base had added inside the code the branch moved. No test installs the whole client, so no test could miss the service.

### Rule

Before resolving a hunk toward the side that moved the code, list every `provide`, `require`, `find` or registration the base added there, check each one is in the merged tree, and give a line no test would miss a test of its own.
