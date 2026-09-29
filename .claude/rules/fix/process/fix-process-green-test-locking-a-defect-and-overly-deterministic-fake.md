---
description: An assertion describes the intended behavior, never the observed output; a test double keeps the risky properties of the real dependency
trigger: always_on
---

# RULE: An assertion describes the INTENDED behavior, never the observed one — and a double tamer than the real dependency makes a whole class of bugs invisible

Why: an assertion copied from the output turns the current behavior into the specification — it passes the day it is written and every day after, including when the behavior is wrong. And a test double defines what the suite can see: every property of the real thing it does not reproduce becomes a blind spot shared by the whole suite.

## Rule

1. **An assertion states what the product requires, not what the code returned.** Before writing `EXPECT_EQ(x, value)`, answer "why is this value the right one?". If the only answer is "that's what it outputs", the assertion is not written.
2. **Distrust assertions that recognize a shape rather than a value** (`EXPECT_FALSE(buffer.empty())`, `EXPECT_GT(size, 0)`): they pass for a whole family of values, wrong ones included. When the expected value is known, assert **equality with the source**.
3. **A fix that turns an existing test red is a signal.** Before adjusting the test, ask whether it was guarding the bug; if so, rewriting it is part of the fix and **is stated in the PR**.
4. **A test double keeps the risky properties of the real dependency.** A UDP transport loses, duplicates and reorders datagrams: the fake transport must be able to. A clock moves forward: the fake clock must move. A double more deterministic than reality is a choice to justify.
5. **Do not modify a shared double for a special case**: inject a local double that reproduces the missing property.
6. **Write the test, watch it fail on the faulty code, then keep it.** A test added on already-fixed code does not prove it catches anything.
7. **Review corollary**: in a test diff, look for assertions describing internal mechanics rather than a fact observable by the player or the protocol.

## Example

- ❌ **Before (wrong)**:
  ```cpp
  EXPECT_EQ(serialize(snapshot).size(), 37);
  ```
  (37 is what the code produced; nobody knows why it is right.)
- ✅ **After (right)**:
  ```cpp
  EXPECT_EQ(deserialize(serialize(snapshot)), snapshot);
  ```
  plus a test of the receiver under a fake transport that **reorders** datagrams, asserting that an older snapshot never overwrites a newer one.
