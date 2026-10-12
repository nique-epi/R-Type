---
description: Pull requests — entirely in English, Conventional Commits title, description following the structure of the repository's PRs
trigger: always_on
---

# RULE: A pull request is written in ENGLISH and follows the structure of the repository's PRs

## Rule

1. **Every pull request is written in English**: title, whole description, section headings, tables, lists, and **every comment posted on the PR** (review, reply, follow-up). No mixing of languages in one document. The language of the conversation has no bearing on what is written in the repository.
2. **The title follows Conventional Commits**, like commits (see `commit.md`).
3. **The description follows the structure of the repository's PRs**, all sections, in order:
   `## Summary` (with `Closes RTY-<n>` for its Linear issue, see `fix-format-pr-links-its-linear-issue.md`) · `## Changes` · `## Type of Change` · `## Testing` · `## Checklist`.
   - If `.github/PULL_REQUEST_TEMPLATE.md` exists, **it is the reference**: read it before opening or editing the PR.
   - Otherwise, mirror the last merged PR (`gh pr view <n> --json body`).
   - The `Type of Change` and `Checklist` boxes are ticked **honestly**: only what is true and verified.
4. **`## Testing` gives the commands actually run and their result** (platform, compiler, output), never an intention.
5. **A PR stacked on another says so** at the top of the Summary (base, and when to retarget it to `main`).
6. **Proper names and identifiers are not translated**: files, branches, CMake targets, symbols.
7. **No AI mention** in the title or description (see `fix-process-no-co-author-trailer.md`).
8. **Reread before sending**: if the title or first section is not in English, the PR is not ready.

## Example

- ❌ **Before (wrong)**:
  ```md
  ## Résumé
  Cette PR ajoute le lecteur de paquets.

  ## Modifications
  - ...
  ```
- ✅ **After (right)**:
  ```md
  ## Summary
  Adds the packet reader that turns a received datagram into a typed message.

  Closes RTY-<n>

  ## Changes
  - ...

  ## Type of Change
  - [ ] Bug fix
  - [x] New feature
  - [ ] Refactor
  - [ ] Documentation

  ## Testing
  macOS arm64 (AppleClang): `cmake --workflow --preset test` -> 14/14 passing, no warnings.

  ## Checklist
  - [x] Code compiles without errors
  - [x] No new warnings introduced
  - [x] Tests pass
  - [x] Self-reviewed the diff
  ```
