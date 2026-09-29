---
description: A measurement only proves a fact if it would have shown something ELSE in the opposite case
trigger: always_on
---

# RULE: A measurement only proves a fact if it would have shown SOMETHING ELSE in the opposite case

## Rule

1. **Before citing a measurement as evidence, ask what it would show if the fact were false.** If the answer is "the same" or "I don't know", it is not evidence.
2. **An unknown metric is calibrated on a known case** before being read on the case to prove.
3. **Prefer structural evidence to a derived metric**: "the test fails on the old code and passes on the new one" proves it catches the bug; "the test passes" proves nothing.
4. **When evidence already announced turns out not to discriminate, say so** in the next message, with the evidence that replaces it.

## Example

- ❌ **Before (wrong)**: "The leak fix is validated: the server runs 10 minutes without crashing." (It already ran 10 minutes without crashing before.)
- ✅ **After (right)**: "Before the fix, server memory grows by 2 MB per minute with 4 clients; after, it stays flat over 10 minutes, same load."
