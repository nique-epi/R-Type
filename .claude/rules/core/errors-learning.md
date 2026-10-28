---
description: Self-correction — create a corrective fix-* rule after a mistake in the code so it cannot happen again; a mistake outside the code produces no rule
trigger: always_on
---

# RULE: Self-correction through new rules — for mistakes in the code only

## Goal

Every time a mistake reaches **the code of the repository**, or would have reached it, a **dedicated corrective rule must be created** to prevent it from happening again. This builds a corrective knowledge base about **this project's code**, shared by the team. Creating the rule is part of handling the mistake, never postponed.

A mistake that only concerns my machine, my tools or the way I work in the conversation **produces no rule**: it is fixed on the spot (see "A mistake outside the code").

## Scope: what counts as the code

**In the code — produces a rule:**

- C++ sources and headers (`src/`);
- tests and test doubles (`tests/`);
- build files: `CMakeLists.txt`, presets, `vcpkg.json`;
- CI workflows (`.github/workflows/`);
- the network protocol and its wire format.

**Outside the code — produces no rule:**

- shell and zsh syntax, quoting, heredocs, scripts I typed;
- git commands: staging, branches, stashes, pushes;
- the local machine: macOS SDK, Homebrew, local tool versions and setup, paths;
- my own checks: isolated copies, red-proof scripts, the local clang-tidy command;
- the docs site, the README, commit messages and pull request text;
- how I answer, plan, report or ask in the conversation.

Test: **would the mistake show in the diff of `src/`, `tests/`, the CMake files or the CI, or in the behavior of the built game?** If not, it is outside the code.

## Triggers

Create a corrective rule when a mistake in the code is:

1. **A reasoning error** — a wrong deduction, two concepts confused or flawed logic that produced a defect in the code.
2. **A defect** — a bug, a crash, a build or CI failure, a warning, a test that cannot fail, a violated `architecture/` or `code-style/` rule.
3. **An interpretation error** — a request about the code misunderstood, or a wrong assumption about the state of the code, that changed what was written.
4. **A regression** — a code mistake that a rule already covers, made again.
5. **A user correction** — the user corrects something in the code, even a minor point.

## A mistake outside the code

1. **Name it in one line, fix it, and go on.** No rule, no history, no index line.
2. **Never a commit, a branch or a pull request for it**, and no extension of an existing rule after a recurrence.
3. **If it is likely to come back**, a one-line note may go in my local memory, outside the repository and never committed.

## Procedure

### Step 1 — Identify and acknowledge

Name the mistake clearly, briefly explain what happened, do not bury it in apologies.

### Step 2 — Create the corrective rule, as two files with the same name

The core is loaded every session; the history is not. Only what prescribes goes in the core; the story of the incident goes in the history.

**Core** — `.claude/rules/fix/[category]/fix-[category]-[short-description].md`:

```markdown
---
description: <one line>
trigger: always_on
---

# RULE: [Short, descriptive title]

Why: [one sentence, only if the rule cannot be understood without it]

## Rule
[Clear, imperative, actionable instruction, usable without extra context]

## Example
- ❌ **Before (wrong)**: [what was done]
- ✅ **After (right)**: [what to do instead]
```

**History** — `.claude/rules-history/fix-[category]-[short-description].md`, never loaded (outside `.claude/rules/`, which Claude Code loads entirely, subdirectories included): the full file, with `## Context`, `## Mistake`, `## Root cause`, then the rule and the example.

**Recurrence or addition**: the new prescription is added to the core under a `### <short title>` subheading of "Rule"; the story is added to the history under `## Update (date) — <title>`.

**When to read the history** — it has the same name as the rule, in `.claude/rules-history/`, and is read on demand:
- before extending a rule after a recurrence (to know what was already tried and failed);
- when the scope of a rule is unclear for the case at hand (the original incident says what it targeted);
- when the user asks why a rule exists.

Rules imported from other projects when R-Type started have no history here: their original incident belongs to another repository.

### Step 3 — Confirm

State the name of the created file, summarize the rule in one sentence, and resume the task applying the correction immediately.

## Rules about rules

- **Granularity**: one rule = one specific mistake. No catch-all rules.
- **Clarity**: the "Rule" section is a simple, self-contained instruction.
- **No duplicates**: before creating a rule, check that none already covers the case. If one does, extend it instead.
- **Consistent naming**: categories `reasoning`, `execution`, `interpretation`, `format`, `process`, `architecture`. The `fix/<category>/` folder carries the category; the `fix-<category>-` prefix stays in the file name.
- **Path scoping**: a rule that only concerns C++ code goes in `architecture/` or `code-style/` with `paths: ["**/*.{cpp,hpp,tpp}"]` in its frontmatter; it is then loaded only when a C++ file is read. Every other rule has no `paths` and is loaded every session. The frontmatter must stay valid YAML (quote the description if it contains `: `), otherwise `paths` is ignored.
- **Language**: English, like the code, commits and pull requests.
- **Public repository**: a rule or a history never contains secrets, personal data or details of another project.
- **Scope**: `fix-*` rules are as binding as the contract rules. Read and apply them before coding or delivering.
- **Index**: every new rule is added to the index in `.claude/CLAUDE.md`.

## Examples

- ❌ **Forbidden**: a test that cannot fail is noticed, I apologize and move on without recording anything — the mistake can happen again.
- ✅ **Instead**: notice the mistake → create `fix/[category]/fix-[category]-[slug].md` (+ its history) → confirm → resume applying the correction.
- ❌ **Forbidden**: a zsh command stops on an unquoted `?` → a new `fix-execution-…` rule, then a branch and a pull request to ship it.
- ✅ **Instead**: one line saying why the command failed, the command rerun quoted, and the work goes on.
