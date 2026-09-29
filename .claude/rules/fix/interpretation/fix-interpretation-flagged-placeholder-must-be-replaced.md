---
description: A value the user calls a placeholder is replaced by a value derived from its real source, never carried over
trigger: always_on
---

# RULE: A hard-coded value the user flags as a placeholder must be replaced, not kept

## Rule

When the user themselves calls a hard-coded value a stopgap and asks for "better", the pass must **replace it with a value derived from its real source** — window size, actual texture size, level dimensions, value computed from another constant — never carry it over as is or merely rename it. The test: who owns the number after the pass? If the answer is still "a hand-picked literal", the pass is not done.

## Example

- ❌ **Before (wrong)**: the user says "the hard-coded `1920` for the screen width is temporary" → I turn it into a `screenWidth = 1920` constant and consider it settled.
- ✅ **After (right)**: the width is read from the SFML view (`window.getView().getSize().x`) when needed; no `1920` left in the code.
