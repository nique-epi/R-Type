---
description: A small feature the user judges by eye or ear gets a verification in proportion, and its cost is said before it starts
trigger: always_on
---

# RULE: A small feature is checked in proportion to its size, and a heavy check is announced first

Why: a click on the menu buttons grew into three fixture refactors and fourteen tests; the user stopped it, since they check a sound by ear.

## Rule

1. **Before the verification of a feature, compare its cost with the change**: a sound, a color or a timing the user hears or sees in the game is checked by the build, the suite and the user's own try.
2. **When the verification needs work of its own** (a fixture to split under a rule, a battery of tests, red proofs), say it in one line with its cost before starting, and let the user choose.
3. **Once the user says they will check it themselves, stop**: keep the build green and the suite passing, write no more test for it, and list in the report what was left to their check.

## Example

- ❌ **Before (wrong)**: the click of the menus → reshape of the menu tests, the connection fixture cut to nine functions, fourteen tests, red proofs planned.
- ✅ **After (right)**: "the click plays on bars and tabs; testing it needs two fixture refactors (about 20 min): do I write them, or do you check it by ear?"
