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

### The expected value is not the constant under test

1. **When a requirement fixes a value** (a mockup position, "one pixel lower", a packet size), the test writes that value from the requirement, as a literal or a test constant named after it (`ONE_PIXEL_LOWER`), never by reading the constant the code under test reads (`PRESSED_TEXT_DROP`). An expected value computed from that constant follows it when it is wrong, and the test stays green.
2. **A constant that only configures the behavior under test** (the places of a formation, when the test checks that the ships reach their places) may be read by the test: what it checks is the behavior, and another test pins the values when the requirement fixes them.
3. **A red proof that changes a constant is the check**: if no test turns red, the value is not pinned anywhere.

### One input per test when the faulty code reaches the same end state

1. **Before asserting only the end of a sequence of inputs**, play the sequence on the faulty reading: if it reaches the same end state through another input (the wrong key opens the tab the right key would have opened), the test cannot fail. Assert after each input, or give each input its own test.
2. **The red proof is the check**: a variant that leaves such a test green means the test is split, never that the variant is dropped.
3. **A call between the input and the assertion can undo what the assertion reads.** Before asserting a state, read every call the test makes after the input for one that resets that state: `acknowledgeIn()` ends the wait that `isAcknowledgementDue()` reads. Assert before such a call, or give each its own test.

### A repeating timer is walked in short steps, and restarted

1. **A test that jumps a whole delay in one update proves the end state, never the pace**: `update(GIVE_UP_TIME)` passes on a timer that is never reset. Walk short frames over the delay and count what went out, with the expected count written from the requirement.
2. **A timer a new request restarts is tested after it expired**: ask again, and check it runs its whole time again.
3. **The red proof removes each reset**, one at a time.

### A color asserted is absent from the other states

1. **Before asserting a look through a color, compare it with the colors of the other states of the widget** (`WidgetColors.hpp`): two constants can hold one color. `BAR_BORDER_COLOR` and `SELECTED_BAR_INSIDE_COLOR` are both `EDGE_BLUE`.
2. **Write the opposite test too**: the state without that look must not contain the color. If it does, the first test could not fail.

### A test of a thread waits until the thread reached the state it tests

1. **A thread starts later than the call that builds it.** A test that stops a job, or adds what its next pass must see, right after building it can act before the first pass: the stop is seen before the loop, the new item before the first pass, and the test passes whatever the loop does.
2. **Before acting, wait for an observable sign that the thread reached the state under test**: its first pass written (a first server online), then the stop or the second item.
3. **The red proof breaks the loop** (one pass only, a sleep that ignores the stop): the test must turn red on each, run several times. A variant left green means the test raced.

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
- ❌ **Before (wrong)**: `ForgetEverythingWhenTheSessionCloses` closed the session, let the delay pass, wrote the acknowledgement into a header, then asserted nothing was due; writing the acknowledgement ends the wait, so a `close()` that kept the wait left the test green.
- ✅ **After (right)**: `ForgetTheWaitWhenTheSessionCloses` asserts the wait right after `close()` and the delay, and `ForgetTheSequencesWhenTheSessionCloses` asserts the header on its own.
