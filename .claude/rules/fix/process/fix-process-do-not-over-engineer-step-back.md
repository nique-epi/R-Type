---
description: Do not over-engineer — find the right way to do it, stop at the second round trip, step back over the whole flow
trigger: always_on
---

# RULE: Do not over-engineer — find the right way to do it, then step back over the whole flow

## Rule

**Before** writing a single line on a poorly understood topic — and **mandatorily** at the second user feedback on the same thing:

1. **STOP at the 2nd round trip.** Two corrections on the same point = the problem is not where I am looking. Stop patching.
2. **Find the right way to do it, never guess**: the library's documentation (SFML, Asio, GoogleTest, CMake), its headers installed by vcpkg, a web search if needed. An unverified technical hypothesis is not coded; it is verified, or stated as a hypothesis.
3. **Step back over the whole flow**, not the faulty line: walk through the feature like a sequence diagram (who calls what, in which order, which module owns what — client, server, engine), reread the already-shipped code around it, and produce three lists: **to refactor / to delete / to add**. Deletion is proposed on the same footing as addition.
4. **Prefer what the library already does over rebuilding it.** Before writing a homemade mechanism (timer, event queue, serialization, window handling), check what SFML, Asio or the standard library provide.
5. **When several iterations make things worse**: go back to the original state (git) and restart from step 2, never stack one more fix.

### A question about a detail does not authorize a redesign

1. **Answer first**, with the finding — without touching the code.
2. **Change only the reported detail.**
3. **Aligning conventions is proposed, not delivered**: "these two modules differ on X, I can align them — do you want that?".
4. A reuse opportunity spotted along the way is a **finding to report**, not a project to start in the same commit.

## Example

- ❌ **Before (wrong)**: the game loop drifts → write a homemade `sleep_for`-based timer, then fix it three times with guessed values.
- ✅ **After (right)**: read the docs of `sf::Clock` and `asio::steady_timer`, set a fixed time step with an accumulator, and delete the homemade timer.
