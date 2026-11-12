---
description: "An integer becomes a double through static_cast, never through braces: double{n} of a non-constant integer is a narrowing error"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: An integer becomes a `double` through `static_cast`, never through braces

Why: `double{shownTick}` with a `std::uint32_t` that is not a constant is a narrowing conversion, which the brace form refuses (`-Wc++11-narrowing`, an error); the build stopped on it.

## Rule

1. **To turn an integer variable into a floating-point value, write `static_cast<double>(n)`** (or `static_cast<float>(n)`), never `double{n}` or `float{n}`: braces refuse any conversion that may lose a value, and a 32-bit or 64-bit integer may not fit exactly.
2. **Braces stay for a literal or a constant the compiler knows fits** (`double{1}`), and for building an aggregate.

## Example

- ❌ **Before (wrong)**: `const double ticks = *serverTick - double{shownTick};` → `non-constant-expression cannot be narrowed from type 'std::uint32_t' to 'double'`.
- ✅ **After (right)**: `const double ticks = *serverTick - static_cast<double>(shownTick);`.
