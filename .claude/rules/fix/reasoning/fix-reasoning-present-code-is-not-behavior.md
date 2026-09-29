---
description: Present code is not proven behavior — require execution evidence before saying "it already exists"
trigger: always_on
---

# RULE: Present code is not behavior — require execution evidence

## Rule

1. **Before answering "it already exists"** about a visible or behavioral feature (a sound, an animation, a packet sent, an enemy spawning), require **execution evidence** — a log, a test going through it, a screenshot, the user's account — not code evidence. Otherwise say "the code is there, I have no evidence it produces the effect", and check.
2. **A declaration is not a call.** A packet type declared in the `enum`, a system registered but never added to the loop, a method with no call site: look for the **call site** (`grep -rn`) before claiming something happens.
3. **Any assumed API behavior is checked in the docs or headers** before being asserted; a homemade doc comment does not count as a source (see `fix-process-instrument-before-reasoning-docs-are-truth.md`).
4. **When a diagnosis goes in circles** (two re-reads concluding "it's correct" against an "it doesn't work"), stop re-reading the wiring and check that the **final building block** produces its effect.

## Example

- ❌ **Before (wrong)**: "the shooting sound already exists, `SoundSystem` handles `ShootEvent`" → the sound never plays: `SoundSystem` is not in the list of executed systems.
- ✅ **After (right)**: `grep -rn "SoundSystem" src` → declared, never added to the loop → say so, then fix the registration.
