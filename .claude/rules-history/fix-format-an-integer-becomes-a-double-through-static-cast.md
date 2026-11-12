# RULE: An integer becomes a `double` through `static_cast`, never through braces

## Context

RTY-267: the client prediction moves the blocking walls ahead by the scroll lead, `scrollLeadOf(scrollSpeed, serverTick, shownTick)`, which subtracts the tick drawn (a `std::uint32_t`) from the tick of the server (a `double`).

## Mistake

The plan, then the code, wrote `*serverTick - double{shownTick}`; the build of `rtype_client_prediction_own_ship` stopped on `-Wc++11-narrowing`: a non-constant `std::uint32_t` cannot be narrowed to `double` in an initializer list.

## Root cause

The brace form was chosen as a "safe" cast, while braces are exactly the form that refuses a conversion the compiler cannot prove exact; the plan's code was never compiled before it was shown.

## Rule

1. To turn an integer variable into a floating-point value, write `static_cast<double>(n)` (or `static_cast<float>(n)`), never `double{n}` or `float{n}`.
2. Braces stay for a literal or a constant the compiler knows fits, and for building an aggregate.

## Example

- ❌ `const double ticks = *serverTick - double{shownTick};`
- ✅ `const double ticks = *serverTick - static_cast<double>(shownTick);`
