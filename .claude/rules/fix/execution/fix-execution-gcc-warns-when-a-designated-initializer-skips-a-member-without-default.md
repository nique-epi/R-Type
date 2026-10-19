---
description: A designated initializer that leaves out a member with no default member initializer fails the GCC build
trigger: always_on
---

# RULE: A designated initializer lists every member that has no default member initializer — GCC rejects the omission, Clang does not

Why: `-Wmissing-field-initializers` with warnings as errors failed the Linux job on a `ConnectedPlayer{...}` that skipped a new `std::optional` member; AppleClang and clang-tidy stayed silent.

## Rule

1. **When a member is added to an aggregate**, grep every `Type{.` that builds it (`git grep -n 'Type{' -- src tests`) and list the new member in each one that does not set it, as `.member = std::nullopt` or `.member = {}`. A member with a default member initializer (`bool ready{false};`) may be skipped; one without may not.
2. **Do not silence it by writing `{}` on the member** when it is of class type: `readability-redundant-member-init` reports it. Name the value at the construction site instead.
3. **The local Clang run is not the verdict**: Clang's `-Wmissing-field-initializers` stayed silent on the failing line, so it does not calibrate this check. State in the report that GCC was not run, and read the Linux job.

## Example

- ❌ **Before (wrong)**: `ConnectedPlayer` gains `std::optional<std::uint16_t> newestReceivedSequence;` and `PlayerRoster::add` keeps `ConnectedPlayer{.playerNumber = …, .nextSequence = 0}` → `missing initializer for member ‘newestReceivedSequence’` on Linux.
- ✅ **After (right)**: `.nextSequence = 0, .newestReceivedSequence = std::nullopt}`.
