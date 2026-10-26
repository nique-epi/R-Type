---
description: A small feature the user judges by eye or ear gets a verification in proportion, and its cost is said before it starts
trigger: always_on
---

# RULE: A small feature is checked in proportion to its size, and a heavy check is announced first

## Context

On 25 October the user asked for a small click when a menu button or tab is pressed, before the stacked pull requests of RTY-123.

## Mistake

The click was wired in one pass, but its tests were not: the menu tests were reshaped into fixtures, the connection screen fixture was cut from twelve member functions to nine so it could take a sound accessor, fourteen tests were written and red proofs were planned. The user stopped it: "arrête de tester le bouton, tu perds trop de temps pour rien, je testerais moi-même si le bouton fait bien le son".

## Root cause

The rules on fixtures and tests were applied in full without weighing them against the change, and the cost of the verification was never told to the user before it started.

## Rule

1. **Before the verification of a feature, compare its cost with the change**: a sound, a color or a timing the user hears or sees in the game is checked by the build, the suite and the user's own try.
2. **When the verification needs work of its own** (a fixture to split under a rule, a battery of tests, red proofs), say it in one line with its cost before starting, and let the user choose.
3. **Once the user says they will check it themselves, stop**: keep the build green and the suite passing, write no more test for it, and list in the report what was left to their check.

## Example

- ❌ **Before (wrong)**: the click of the menus → reshape of the menu tests, the connection fixture cut to nine functions, fourteen tests, red proofs planned.
- ✅ **After (right)**: "the click plays on bars and tabs; testing it needs two fixture refactors (about 20 min): do I write them, or do you check it by ear?"
