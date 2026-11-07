---
description: Code whose result must stay within a range is checked on empty input, full input and both bounds — never only the nominal case
trigger: always_on
---

# RULE: A bounded contract is checked on the EMPTY state and at both bounds — never only the nominal case

## Rule

Before shipping code whose result must stay **within a range** — index in a buffer, read offset in a packet, entity id, player slot, position in a ring buffer:

1. **Walk through the boundary cases by hand**: **empty** input, **full** input, and one step on each side of each bound. Write the expected result for each before coding. Most of these contracts break on the empty case, never on the nominal one.
2. **Bound explicitly on the data you answer for**, rather than relying on an assumed invariant between two independent lengths (size announced in the header vs bytes received).
3. **Write the boundary test, watch it fail on the faulty code, then keep it.**

### A range written in words copies the computed endpoints

1. **When a range goes into prose** (a page, a rule, a doc comment), **write its endpoints as numbers, taken from the values computed or tested**, with the edge that is excluded named: "1 to 32767; 32768 is never newer", not "1 to half the range".
2. **Reread the sentence against the boundary test**: each endpoint in the text matches a case of the test, and the excluded value is in neither.

### A guard on a size is tested where it changes the result

1. **Before writing the test of a guard such as `std::max(0, size - 1)`, compute the guarded and the unguarded value at each candidate size.** The test uses a size where they differ: size 0 for `max(0, size - 1)`, never size 1, where both give 0.
2. **Find the smallest size the callers really pass**, by reading them: a box whose corners close passes a height of 0.
3. **Compute both values in the type the code computes in**, not in the type of its result: a shift done in 64 bits and cut to 32 gives 0 with or without a bound on 32 for every distance up to 64. They differ only past the width of the computation, where the unguarded shift is undefined behavior: test there, and say in the report that the red proof goes through undefined behavior.

### A guard on a cast is tested past the width of the cast

1. **A guard that keeps a wide number out of a narrow type** (`place < 0 || place > 65535` before a `static_cast<std::uint16_t>`) **is tested with values whose cut lands on a valid value**: `65537` and `-65535` both cut to `1`. A value such as `-1` cuts to `65535`, which the lookup refuses anyway, so the test passes without the guard.
2. **A check on the number of words is tested with one word too many**, not only with none: `words.size() != 1` and `words.empty()` both refuse an empty line.

## Example

- ❌ **Before (wrong)**: `readUint16(buffer, offset)` tested on a full 64-byte packet only → out-of-bounds read on a 1-byte datagram.
- ✅ **After (right)**: tests on 0, 1, header size − 1, header size and maximum size; anything shorter than the header is rejected before reading, test verified red on the old version.
- ❌ **Before (wrong)**: the comparison was tested with 32767 newer and 32768 not newer, then the rule said "newer means ahead by 1 to half the range", which includes 32768.
- ✅ **After (right)**: "newer means ahead by 1 to 32767; exactly half the range (32768) is never newer".
- ❌ **Before (wrong)**: the guard `lower = top + max(0, height - 1)` of `paintBoxCorners` tested on a box one row tall, where the unguarded `top + height - 1` gives the same row: the test could not fail.
- ✅ **After (right)**: the test paints a closed box, 0 rows tall, as the squadron passes when its corners meet; without the guard the bottom strokes land one row above the box.
- ❌ **Before (wrong)**: the bound `distance > 32` of `earlierAfterMoving` tested by a jump of 33 sequences; the bits move in 64 bits, so the unguarded code also gave 0 and the red proof left every test green.
- ✅ **After (right)**: a jump of 65, the first past the 64 bits of the shift; without the bound the code shifts by 64, keeps a bit on arm64, and the test turns red, through undefined behavior.
