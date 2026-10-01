---
description: A spotted inconsistency is reworked or reported, even if pre-existing — never silently extended
trigger: always_on
---

# RULE: A spotted inconsistency gets REWORKED, even if it is pre-existing — never silently extended

Why: "minimal diff" is about functional scope, not quality. Every extension of a bad pattern raises the cost of removing it.

## Rule

1. **Any spotted inconsistency** (violation of the repository conventions or of `.claude/rules/`: method body in a `.hpp`, raw `throw std::`, magic value, off-convention naming, bypassed layer, SFML dependency on the server side…) **is dealt with, even if it predates the current work.** "The file already did it this way" is never a reason to extend it.
2. **Assess the impact before acting**:
   - **Contained impact** (current file or module, identical behavior, provable by the build and tests) → **refactor directly**, in a `refactor` commit separate from the functional change.
   - **High impact** (several modules, network protocol, game loop, risk of behavior change, a teammate currently working on it) → **STOP, ask**, with the finding, the rework option and its cost.
   - **Unsure where the line is** → ask. Asking is always allowed; silently extending never is.
3. **The inconsistency is always reported** in the PR or report, even when the rework is deferred by decision.
4. Also applies to **subagents**: their briefs carry this rule.

## Example

- ❌ **Before (wrong)**: the file defines its getters in the `.hpp` → I add mine in the `.hpp` "to stay consistent with the file".
- ✅ **After (right, contained impact)**: I move the existing bodies into the `.cpp` (dedicated `refactor` commit), then add my method in the right place.
- ✅ **After (right, high impact)**: "The packet format duplicates its sizes on the client and the server. Unifying them touches both binaries and the protocol. (a) I do it now in a dedicated commit, (b) I add my packet minimally and we unify right after. Which do you prefer?"
