# RULE: A designated initializer lists every member that has no default member initializer

## Context

The game states story added `bool ready{false}` and `std::optional<std::uint16_t> newestReceivedSequence` to `ConnectedPlayer`. `PlayerRoster::add` builds a `ConnectedPlayer` with designated initializers.

## Mistake

The optional member first had a `{}` initializer, which clang-tidy 21 reported as `readability-redundant-member-init`; removing it made the local tidy pass, but GCC on the Linux job then failed with `-Werror=missing-field-initializers` on `PlayerRoster.cpp:56`. The local AppleClang build and tests were green, and the construction site was never reread after the member was added.

## Root cause

Two checks pull in opposite directions (tidy wants no initializer on a class-type member, GCC wants every member without one named), and only one of them runs locally. The fix of one finding was not followed by a search for the other toolchain's view of the same line.

## Rule and example

See the core file of the same name in `.claude/rules/fix/execution/`.
