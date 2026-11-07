# RULE: A designated initializer lists every member that has no default member initializer

## Context

The game states story added `bool ready{false}` and `std::optional<std::uint16_t> newestReceivedSequence` to `ConnectedPlayer`. `PlayerRoster::add` builds a `ConnectedPlayer` with designated initializers.

## Mistake

The optional member first had a `{}` initializer, which clang-tidy 21 reported as `readability-redundant-member-init`; removing it made the local tidy pass, but GCC on the Linux job then failed with `-Werror=missing-field-initializers` on `PlayerRoster.cpp:56`. The local AppleClang build and tests were green, and the construction site was never reread after the member was added.

## Root cause

Two checks pull in opposite directions (tidy wants no initializer on a class-type member, GCC wants every member without one named), and only one of them runs locally. The fix of one finding was not followed by a search for the other toolchain's view of the same line.

## Rule and example

See the core file of the same name in `.claude/rules/fix/execution/`.

## Update (3 November 2026) — a construction with a name

`MetricsReport` gained a member with no default. The one `MetricsReport{` the search found was fixed, and the build and the 2916 tests stayed green under Apple clang, which is silent on it. An independent review found the three `const MetricsReport report{…}` of `MetricsLinesTest.cpp`, which would have failed the Linux job. Homebrew clang 21 with `-Wmissing-designated-field-initializers` reports them, so it gives a local check where the rule had said no local Clang could.

## Update (3 November 2026) — A NOLINT over the same braces

`LeaderboardPage::rows` and `ItemCell::tag` wrote `{}` with a `NOLINT` for `readability-redundant-member-init`, the way out point 2 forbids, copied from `Button.hpp`. The page now names `.rows = {}` where it is built, and the tag is an optional.

## Update (5 November 2026) — The search was not the pattern of the rule

`EntityAppearance` gained `actions`. The search used was `git grep 'EntityAppearance{'`, which found the double of the tests; `static const EntityAppearance appearance{` of `EntitySpritesFixtures.cpp` has a name between the type and the brace, so the Linux job failed on it with `-Werror=missing-field-initializers`, while the Apple clang build, the tests and clang-tidy were green. The rule already said to search `Type name{`. Homebrew GCC 16 turned out to be installed and, with the macOS sysroot and `-Wall -Wextra -Wpedantic`, reproduces the warning: it is now the local check.

## Update (7 November 2026) — A member removed

A simplification pass removed `BossFight::bossEntityId`, which nothing read. The search `git grep -n 'bossEntityId'` listed `tests/client/Widgets/BossBarFixtures.cpp:14`, where `fightWith()` builds a `BossFight{.bossEntityId = BAR_BOSS_ID, …}`, among a dozen hits of the network message `BossStatus`, whose member has the same name; the hit was read as one of the message. The build stopped on `field designator 'bossEntityId' does not refer to any field in type 'rtype::client::BossFight'`. The fixture lost the line and `BAR_BOSS_ID`, used nowhere else.
