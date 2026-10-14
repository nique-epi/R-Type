---
description: "Before predicting that AddressSanitizer catches a bad access, check that the access leaves its allocation"
trigger: always_on
---

# RULE: AddressSanitizer only sees an access that leaves its allocation — an overflow into the next member of the same object is invisible to it

## Rule

1. **Before predicting "ASan reports it"** for an out-of-bounds access, name the allocation the access starts in (a heap block, a stack variable, a global) and check that the access leaves it. An index past a member array that lands in the next member of the same object stays inside the allocation: ASan says nothing.
2. **A red proof that relies on a sanitizer reads the sanitizer's report, not only the test result.** A test that fails with no report failed through corrupted memory: that is undefined behavior, and another build may pass. Say so next to the result.
3. **A measurement taken in one memory layout does not carry over to another**: an overflow reported for an object alone on the stack is not reported once the object is a member followed by other members.
4. **When the proof must be reliable**, give the object its own allocation for the test (for example `std::make_unique`), so the overflow leaves it and ASan can see it.

## Example

- ❌ **Before (wrong)**: the plan predicts "without the layer guard, ASan reports the overflow" because a review once measured it; the test holds `RenderSystem` inside a `Scene` followed by a `RecordingSurface` → the test turns red, ASan reports nothing.
- ✅ **After (right)**: "index 4 of `drawCallsByLayer_` lands in the next member of `Scene`, inside the same object: ASan cannot see it. The test turned red through corrupted data, which is undefined behavior; for a sanitizer-backed proof, allocate the system on its own."
