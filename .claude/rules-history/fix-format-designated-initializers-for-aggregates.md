# RULE: An aggregate with several members is built with designated initializers

## Context

`Entity` gained a second member (`index` and `generation`). The implementation plan, validated by the user, built it positionally in `EntityRegistry.cpp` and in a test.

## Mistake

The plan and the first implementation wrote `Entity{index, generations_[index]}` and `Entity{0, 0}`. `clang-tidy` rejected both with `modernize-use-designated-initializers`, treated as an error.

## Root cause

The plan was written without checking which `modernize-*` checks the repository's `.clang-tidy` enables (`Checks: '*'` minus a short list). The previous single-member `Entity{nextId_++}` did not trigger the check, so nothing in the existing code showed the constraint.

## Rule

1. Build an aggregate that has two or more members with designated initializers, in declaration order: `Type{.first = a, .second = b}`.
2. This applies to the code shown in an implementation plan too: the plan is validated on code that passes the gates.

## Example

- ❌ **Before (wrong)**: `return Entity{index, generations_[index]};`
- ✅ **After (right)**: `return Entity{.index = index, .generation = generations_[index]};`
