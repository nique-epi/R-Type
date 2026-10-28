# RULE: Run `/simplify` at the end of every plan application

## Goal

A plan applied step by step leaves traces of the path taken: a helper used once, a duplicated block, a parameter that no caller varies. `/simplify` reviews the changed code for reuse, simplification, efficiency and altitude, then applies the fixes. It runs once the whole plan is applied, never between steps.

## Rule

1. **When the last step of an approved plan is done**, invoke the `simplify` skill (`/simplify`) on the resulting diff before announcing the work finished.
2. **It runs before the final gates**: `cmake --workflow --preset test`, `format-check` and `tidy` are run (or rerun) on the code `/simplify` leaves, never on the code it started from.
3. **It is a step of the plan application, not of every small edit.** A one-line fix or an answer to a question does not trigger it; applying a plan does.
4. **Report what it changed** in the final answer: the simplifications applied, or "nothing to simplify". A simplification that changes behavior or a public contract is stopped and put to the user, as `fix-process-rework-preexisting-inconsistencies.md` requires for high-impact changes.
5. **`/simplify` hunts quality, not bugs.** It does not replace the review of `core/code-review.md`.
6. **Commits stay atomic**: the simplifications go in the commit of the step they simplify, or in a separate `refactor` commit when they cut across steps.

## Example

- ❌ **Before (wrong)**: the last step of the plan is done, tests are green, "plan applied" is announced; a helper called from one place and a copied loop remain.
- ✅ **After (right)**: last step done → `/simplify` → the copied loop is merged into the existing helper → tests, `format-check` and `tidy` run on the simplified code → the answer lists what `/simplify` changed.
