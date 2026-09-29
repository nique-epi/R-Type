---
description: An agent report is relayed separating what it verified itself from what it delegated or assumed
trigger: always_on
---

# RULE: Never relay the facts of an agent report without separating what it VERIFIED from what it delegated or assumed

Why: a dense, structured report full of quotes and references imitates the shape of sourced work; density costs nothing to produce without the sources.

## Rule

1. **Require the distinction in the brief.** Every delegated research or audit brief asks, for each claim: *verified first-hand* (with the file read, the command or URL actually called), *reported by a subagent*, or *not verified*. A report without this distinction is incomplete, whatever its apparent quality.
2. **Forbid writing a section whose source did not answer.** A subagent that returns nothing produces a "not covered" section, never a section written from memory.
3. **Never relay a number, a quote or an API behavior without knowing who read it.** Decision-bearing claims are either verified by me, or explicitly attributed ("the agent reports, not verified").
4. **Be twice as careful when the report is used to correct me.** A source that overturns my position deserves stricter verification, not less.
5. **Cascading delegation is one more point of failure**: the brief requires that a subagent's result be quoted as is, and its absence stated if it occurs.

## Example

- ❌ **Before (wrong)**: the agent writes "Asio guarantees that handlers of the same `strand` never run concurrently, and SFML 3 is thread-safe for rendering"; I relay both as established.
- ✅ **After (right)**: brief requiring the verified/delegated/not-verified marking; when relaying, the `strand` guarantee (read in the Asio docs, link cited) is presented as established, the SFML claim as not verified — and I check it before making it a decision.
