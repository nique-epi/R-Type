# History: follow the CI after opening a PR

## Context
A draft PR was opened and the answer ended with the local results and "the CI covers GCC and MSVC". The CI was not read afterwards.

## Mistake
The PR was reported as delivered without looking at the state of its checks.

## Root cause
No rule required reading the CI after `gh pr create`; the local build was treated as enough.

## Rule
See `.claude/rules/fix/process/fix-process-follow-ci-after-opening-a-pr.md`.
