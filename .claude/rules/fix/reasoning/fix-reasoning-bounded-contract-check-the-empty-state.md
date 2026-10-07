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

## Example

- ❌ **Before (wrong)**: `readUint16(buffer, offset)` tested on a full 64-byte packet only → out-of-bounds read on a 1-byte datagram.
- ✅ **After (right)**: tests on 0, 1, header size − 1, header size and maximum size; anything shorter than the header is rejected before reading, test verified red on the old version.
- ❌ **Before (wrong)**: the comparison was tested with 32767 newer and 32768 not newer, then the rule said "newer means ahead by 1 to half the range", which includes 32768.
- ✅ **After (right)**: "newer means ahead by 1 to 32767; exactly half the range (32768) is never newer".
