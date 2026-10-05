---
description: Aggregates with several members are built with designated initializers, as clang-tidy requires
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: An aggregate with several members is built with designated initializers

Why: `modernize-use-designated-initializers` is enabled in `.clang-tidy` with warnings as errors; a positional `Type{a, b}` fails the `tidy` gate.

## Rule

1. Build an aggregate that has two or more members with designated initializers, in declaration order: `Type{.first = a, .second = b}`.
2. This applies to the code shown in an implementation plan too: the plan is validated on code that passes the gates.

## Example

- ❌ **Before (wrong)**: `return Entity{index, generations_[index]};`
- ✅ **After (right)**: `return Entity{.index = index, .generation = generations_[index]};`
