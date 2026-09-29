---
description: Commits — Conventional Commits in English, fixed types, optional but never vague scope, one commit = one intent
trigger: always_on
---

# RULE: Plain Conventional Commits in English

## Goal

Every commit made by the agent **strictly** follows Conventional Commits, in **English**, and carries **a single intent**. A non-compliant message does not go out: fix it before committing.

The repository history is the reference for style (`git log --format=%s`): short subjects, imperative mood, no scope so far.

## Format

```text
<type>[(<scope>)][!]: <description>

[body]
```

## Allowed types

`feat`, `fix`, `docs`, `style`, `refactor`, `perf`, `test`, `build`, `ci`, `chore`, `revert`. No other type.

| Type | When |
|---|---|
| `build` | CMake, presets, vcpkg, dependencies |
| `ci` | GitHub Actions workflows |
| `test` | adding or fixing tests only |
| `chore` | tooling, lint/format configs, repository files with no effect on the binaries |

## Scope

- **Optional.** The current history uses none; stay consistent within a PR.
- When used, it names a **real area of the repository** (`server`, `client`, `engine`, `network`, `ecs`… depending on the layout at the time), in lowercase.
- Never a vague scope: `misc`, `stuff`, `various`, `update`, `changes`, `project`, `tmp`.
- If a commit mixes two unrelated areas, **split it** instead of looking for a broad scope.

## Description

1. English, imperative present (`add`, `fix`, `wire`), lowercase first letter, no trailing period.
2. States the **effect** of the commit, not the list of files.
3. No reference to a ticket, task or plan phase (see `code-style-comments-english-outside-bodies-no-tickets.md`).
4. The body, if any, explains **why**; lines ≤ 100 characters.

## Breaking change

`!` after the type (or scope) **and** a `BREAKING CHANGE: <explanation>` line in the body — typically a change to a network packet format.

## Checks before every commit

1. type is in the list;
2. scope is absent, or real and precise;
3. description in English, imperative, no trailing period;
4. **no AI trailer or mention** (see `fix-process-no-co-author-trailer.md`);
5. the index contains exactly what the message announces (see `fix-execution-check-the-index-before-committing.md`).

## Examples

Right:

```text
build: pin vcpkg and declare third-party dependencies
feat(network): serialize player input packets
fix(server): drop packets shorter than the header
test: cover the entity registry removal path
```

Wrong:

```text
Update stuff
feat: Added the packet parser.
fix(misc): various fixes
feat(network): add packet parser (TASK-12)
```
