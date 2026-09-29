---
description: Self-correction — create a corrective fix-* rule after every mistake so it cannot happen again
trigger: always_on
---

# RULE: Self-correction through new rules

## Goal

Every time a mistake is made — in reasoning, execution, interpretation, format or behavior — a **dedicated corrective rule must be created** to prevent it from happening again. This continuously builds a corrective knowledge base specific to **this project**. The rule is **always active**: creating the corrective rule is part of handling the mistake, never postponed.

## Triggers

Create a corrective rule when:

1. **Reasoning error** — wrong deduction, two concepts confused, flawed logic.
2. **Execution error** — a script or command fails because of a syntax, path or parameter I generated.
3. **Interpretation error** — the request was misunderstood, or a wrong assumption was made about the context or the state of the code.
4. **Format / output error** — deliverable in the wrong format or structure, conventions not followed.
5. **Process error** — step forgotten, wrong order, explicit constraint ignored.
6. **Regression** — repeating a mistake that was already reported or fixed.
7. **User correction** — the user explicitly corrects something, even a minor point.

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

- ❌ **Forbidden**: noticing a mistake, apologizing, and moving on without recording anything — the mistake can happen again.
- ✅ **Instead**: notice the mistake → create `fix/[category]/fix-[category]-[slug].md` (+ its history) → confirm → resume applying the correction.
