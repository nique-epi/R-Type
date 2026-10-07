# History: a summary keeps the conditions of the rule it summarizes

## Context

The protocol page has a table of the checks a receiver runs on each datagram, followed by a paragraph stating the anti-amplification rule: the server never sends an unknown sender more bytes than the datagram it answers.

## Mistake

The table row for the version check said the server answers an unknown sender with `ConnectionRefused (IncompatibleVersion)`, unconditionally. The smallest valid datagram is 25 bytes and that answer is 26, so the row contradicted the paragraph under it. The pull request review flagged it.

## Root cause

I wrote the condition once, in the general paragraph, and kept the table row short. A reader of the row alone, typically the one implementing that check, would follow the row.

## Rule

A row, item or box that restates a rule carries its conditions; after editing either, reread the other; when the condition is long, point to it instead of shortening it.

## Example

- ❌ **Before (wrong)**: "dropped; the server answers an unknown sender with `ConnectionRefused` (`IncompatibleVersion`)".
- ✅ **After (right)**: "dropped; the server answers an unknown sender with `ConnectionRefused` (`IncompatibleVersion`) if the datagram holds at least 26 bytes".

## Update (2026-10-07) — The clauses of one rule are checked against each other

### Mistake

The first version of this rule said in clause 1 that a summary carries the conditions of its rule, and in clause 3 that it may say "see below" instead. The two clauses contradicted each other, in a rule about summaries contradicting their rules. The pull request review flagged it.

### Root cause

Clause 3 was added as an alternative without going back to clause 1 to make room for it.

### Rule

A summary either carries the conditions or points to the rule that states them; the clauses of one rule are checked against each other like a summary against its rule, and a clause that allows an exception to an earlier one is named as such in the earlier clause.
