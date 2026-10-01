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

## Example

- ❌ **Before (wrong)**: `readUint16(buffer, offset)` tested on a full 64-byte packet only → out-of-bounds read on a 1-byte datagram.
- ✅ **After (right)**: tests on 0, 1, header size − 1, header size and maximum size; anything shorter than the header is rejected before reading, test verified red on the old version.
