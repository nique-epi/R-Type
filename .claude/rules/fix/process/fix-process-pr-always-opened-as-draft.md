---
description: Every PR the agent creates is opened as a draft; only the user marks it ready for review
trigger: always_on
---

# RULE: Every PR I create is opened as a draft, never ready for review

## Rule

1. **Every PR I open goes through `gh pr create --draft`**, no exception — one-line change, full feature or fix. Never a bare `gh pr create`.
2. **I never mark a PR as "ready" myself** (`gh pr ready`): the user does it once they have reviewed and validated it.
3. On an existing PR, I do not touch its status unless explicitly asked.
4. A PR opened as "ready" by mistake is fixed immediately: `gh pr ready <n> --undo`.
5. **State the status in the answer**: "PR opened as a draft".

## Example

- ❌ **Before (wrong)**: `gh pr create --title "…" --body "…"` → PR opened ready for review, before any validation.
- ✅ **After (right)**: `gh pr create --draft --title "…" --body "…"` → the user marks it "ready" after review.
