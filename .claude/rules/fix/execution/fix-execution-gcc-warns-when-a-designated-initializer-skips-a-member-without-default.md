---
description: A designated initializer that leaves out a member with no default member initializer fails the GCC build
trigger: always_on
---

# RULE: A designated initializer lists every member that has no default member initializer — GCC rejects the omission, Clang does not

Why: `-Wmissing-field-initializers` with warnings as errors failed the Linux job on a `ConnectedPlayer{...}` that skipped a new `std::optional` member; AppleClang and clang-tidy stayed silent.

## Rule

1. **When a member is added to an aggregate**, grep every `Type{.` that builds it (`git grep -n 'Type{' -- src tests`) and list the new member in each one that does not set it, as `.member = std::nullopt` or `.member = {}`. A member with a default member initializer (`bool ready{false};`) may be skipped; one without may not.
2. **Do not silence it by writing `{}` on the member** when it is of class type: `readability-redundant-member-init` reports it. Name the value at the construction site instead. A `// NOLINT` above the braces is the same way out, not another one, and neighbouring code that does it is reported, not copied. A member that may stay absent can be a `std::optional` with `= std::nullopt`, which needs neither.
3. **The local Clang run is not the verdict**: Clang's `-Wmissing-field-initializers` stayed silent on the failing line, so it does not calibrate this check. State in the report that GCC was not run, and read the Linux job.

### A construction with a name is a construction too

1. **The search of point 1 also looks for `Type name{`**: `git grep -n -E '\bType( [A-Za-z_]+)?\{' -- src tests`. A search for `Type{` misses `const Type report{…}`.
2. **Homebrew clang reports what GCC reports, where Apple clang stays silent**: before the push, compile every file that names the aggregate with `/opt/homebrew/opt/llvm/bin/clang++ -fsyntax-only -Wmissing-designated-field-initializers` and the flags of its compile command. Calibrated on `MetricsLinesTest.cpp` with a member left out: three warnings there, none on a file that lists the member. It only narrows what is left to find on the Linux job.

### Homebrew GCC gives the verdict of the Linux job

1. **Run the search of the previous subsection with its exact pattern**, `\bType( [A-Za-z_]+)?\{`, never a shorter one: `EntityAppearance{` found the double of the tests and missed `static const EntityAppearance appearance{` of `EntitySpritesFixtures.cpp`, which failed the Linux job.
2. **Homebrew GCC (`g++-16`) is installed and reports the warning itself**: take the compile command of each file that names the aggregate from `build/compile_commands.json`, replace the compiler by `/opt/homebrew/bin/g++-16 -isysroot "$(xcrun --show-sdk-path)" -D_GNU_SOURCE -Wall -Wextra -Wpedantic`, and replace `-o` and `-c` by `-fsyntax-only`. Without the `-isysroot` GCC finds no system header; without the warning flags it stays silent, since the build's own flags do not carry them for this target.
3. **Calibrate it on the faulty version first**: the file as it was at the failing commit must print `missing initializer for member`, and the fixed one nothing. Calibrated on `EntitySpritesFixtures.cpp` (`actions` left out): one warning on the old file, none on the 50 files of the branch after the fix.

### A member removed is searched by its designator, in tests too

1. **Before removing a member of an aggregate, search its designator**: `git grep -n '\.<member> =' -- src tests`. Each hit that builds that type (not another type with a member of the same name) loses the line, and a test constant used only there goes with it.
2. **Read every hit against the type it builds**: a name shared by the message and the struct the client keeps of it (`BossStatus::bossEntityId`, `BossFight::bossEntityId`) puts both in one list, and a hit of the struct reads as one of the message.

### The GCC run covers every file that builds the aggregate, not the diff of the branch

1. **Take the files to run `g++-16` on from the search of the first point**, every `.cpp` that builds the extended aggregate, never from `git diff --name-only origin/main...HEAD`: a fixture the branch never touched still builds it.

### The search finds a named variable too

1. **An aggregate is also built as a named variable**: `static const Type name{.member = …}`, which a search for `Type{` misses. Search both shapes in one command: `git grep -n -E 'Type( [A-Za-z_]+)?\{' -- src tests`.
2. Calibration: on the branch that added the deaths of the enemies, `git grep -n -E 'EntityAppearance( [A-Za-z_]+)?\{' -- src tests` lists five constructions, `EntitySpritesFixtures.cpp:14` (`static const EntityAppearance appearance{`) among them; `git grep -n 'EntityAppearance{'` lists that one neither.

## Example

- ❌ **Before (wrong)**: `ConnectedPlayer` gains `std::optional<std::uint16_t> newestReceivedSequence;` and `PlayerRoster::add` keeps `ConnectedPlayer{.playerNumber = …, .nextSequence = 0}` → `missing initializer for member ‘newestReceivedSequence’` on Linux.
- ✅ **After (right)**: `.nextSequence = 0, .newestReceivedSequence = std::nullopt}`.
- ❌ **Before (wrong)**: `MetricsReport` gains `network::TrafficReport traffic;`; `git grep 'MetricsReport{'` finds one site, and the three `const MetricsReport report{…}` of `MetricsLinesTest.cpp` stay as they were.
- ✅ **After (right)**: the search finds the four sites, each lists `.traffic = {}`, and the Homebrew clang run prints nothing.
- ❌ **Before (wrong)**: `EntityAppearance` gains `std::vector<ActionAnimation> actions;`, `git grep 'EntityAppearance{'` finds the double of the tests, and `static const EntityAppearance appearance{` of `EntitySpritesFixtures.cpp` keeps three members.
- ✅ **After (right)**: the search with the pattern of the rule finds both, and the `g++-16` run over the files of the branch prints nothing.
