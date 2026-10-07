---
description: A table row, list item or diagram box that summarizes a rule written elsewhere carries the rule's conditions or points to them, so the two never contradict each other
trigger: always_on
---

# RULE: A summary keeps the conditions of the rule it summarizes

## Rule

1. **A table row, a list item or a diagram box that restates a rule written elsewhere either carries its conditions or points to the rule that states them**: the "if", the "only when", the sizes and the limits. "The server answers" and "the server answers if the datagram holds at least 26 bytes" are two different contracts; a shorter version that drops the condition is never an option.
2. **After writing or editing either the rule or its summary, reread the other one** and check that a reader of the summary, following its pointer when it has one, would do the same thing as a reader of the rule.
3. **Point rather than shorten when the condition is long**: the summary says "see below" instead of a shorter, wrong version.
4. **The clauses of one rule are checked against each other the same way**: a clause that allows an exception to an earlier one names it as an exception, in the earlier clause.

## Example

- ❌ **Before (wrong)**: the checks table says "the server answers an unknown sender with `ConnectionRefused`", while the paragraph below says it never sends more bytes than it received (a 25-byte datagram would get a 26-byte answer).
- ✅ **After (right)**: the row says "if the datagram holds at least 26 bytes".
