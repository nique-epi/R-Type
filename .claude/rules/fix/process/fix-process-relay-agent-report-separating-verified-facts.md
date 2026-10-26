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

### The first sentence is never stronger than the report

1. **The opening of the answer carries the weakest verdict it rests on.** When the report marks a point "not verified" or "the docs do not say", the first sentence says "the documentation does not mention it", never "there is none". A correct "not verified" section below does not repair a wrong opening: the user reads the opening.
2. **An absence in the documentation is not an absence in the product.** What depends on the account (an included credit, a quota, a plan option, a feature rolled out to some users) can exist only on the user's own settings page. Before saying it does not exist, name that page as the place that settles it, or ask the user to look.
3. **A third-party source that contradicts the docs' silence is a lead, not noise**: it goes in the opening, next to the silence, never only at the bottom of the report.

## Example

- ❌ **Before (wrong)**: the agent writes "Asio guarantees that handlers of the same `strand` never run concurrently, and SFML 3 is thread-safe for rendering"; I relay both as established.
- ✅ **After (right)**: brief requiring the verified/delegated/not-verified marking; when relaying, the `strand` guarantee (read in the Asio docs, link cited) is presented as established, the SFML claim as not verified — and I check it before making it a decision.
- ❌ **Before (wrong)**: the report says "separate cloud credit: NOT VERIFIED, the official pages do not mention one; third-party posts claim a one-time $100 (Pro) or $250 (Max) credit"; my answer opens with "No, there are no separate cloud credits"; the user's usage page shows a "Cloud session credits" block with an included credit.
- ✅ **After (right)**: "The documentation says cloud sessions count against your plan's limits and does not mention a separate credit; third-party posts mention a one-time credit. Your usage page (claude.ai/settings/usage) settles it: if it shows a cloud session credit, that is used first."
