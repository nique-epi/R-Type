---
description: Ask the closed questions that cut the hypothesis tree before writing a diagnostic protocol
trigger: always_on
---

# RULE: Ask what the user already observes BEFORE building a diagnostic protocol

## Rule

1. Before writing a diagnostic protocol, a runbook or broad instrumentation, **first ask the one to three closed questions that cut the hypothesis tree in half** — and wait for the answer. Typically: "at which step does it stop?", "does X happen, yes or no?", "did it ever work?", "on Linux, Windows, or both?".
2. Then instrument **the remaining half of the tree**, not the whole tree. A protocol whose steps the user can half cross out from memory was written too early.
3. When the symptom is described in one sentence, **restate what you understand** and get it confirmed before costly steps (full rebuild of vcpkg dependencies, reinstalling tools).

## Example

- ❌ **Before (wrong)**: "the client can't connect" → six-step runbook including "delete `build/`, reconfigure, check the firewall".
- ✅ **After (right)**: "does the server log the incoming connection?" → "yes" → the problem is on the reply side: instrument only the sending of the acknowledgment and its reception.
