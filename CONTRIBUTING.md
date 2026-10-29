# Contributing

This guide sums up what a pull request must respect to be accepted on the first try: how to branch, how to write a commit, how the code and its tests are written, what the review looks at, and when a change is done. It is the readable version of the rules in `.claude/rules/`, which state each point in full with a wrong and a right example. The rules are the reference: when this guide and a rule disagree, the rule wins and this guide is fixed.

The rules grow from mistakes. Each file of `.claude/rules/fix/` was written after one, and the file of the same name in `.claude/rules-history/` tells what happened.

## Before you start

- Check that the work is not done already: search the code, and the open pull requests (`gh pr list`) for a teammate already on it.
- Decide where the code goes before writing it: the first table of [Project layout](https://nique-epi.github.io/R-Type/project-layout/#where-does-a-new-class-go) answers in five questions.

## Branches

One branch per intent, started from the remote `main`, never from the branch you are on:

```bash
git fetch origin main
git checkout -b feat/bydo-waves origin/main
git log --oneline origin/main..HEAD
```

The last command must print nothing: a new branch carries no commit yet.

- A branch is named `<type>/<slug>`, the type being that of its commits: `feat/bydo-waves`, `fix/tidy-headers`, `docs/engine-page`, `chore/build-system`, `ci/build-and-test`.
- Never commit on `main`, nor on a branch made for other work: a fix found along the way gets a branch of its own, and two unrelated fixes get two.
- A branch whose pull request is merged is finished: a commit added to it is lost. Start a new branch from `origin/main`.
- Commit what belongs to a branch before switching: uncommitted changes follow you to the other branch.

## Commits

Commits follow [Conventional Commits](https://www.conventionalcommits.org/), in English, one intent per commit:

```text
<type>[(<scope>)][!]: <description>

[body]
```

| Type | For |
|---|---|
| `feat` | a new behavior |
| `fix` | a bug fixed |
| `refactor` | code changed without changing what it does |
| `perf` | the same behavior, faster or lighter |
| `test` | tests added or fixed, and nothing else |
| `docs` | the site or the README |
| `style` | formatting only |
| `build` | CMake, presets, vcpkg, dependencies |
| `ci` | the GitHub Actions workflows |
| `chore` | tooling, lint and format configurations, repository files with no effect on the binaries |
| `revert` | an earlier commit undone |

- The description is in the imperative, starts lowercase, has no final period, and says the effect, not the files: `feat: send the Bydos in waves of rising difficulty read from a TOML file`.
- The scope is optional and the history rarely uses one. When used, it names a real area in lowercase (`server`, `client`, `engine`, `network`), never `misc` or `update`. A commit that spans two unrelated areas is split instead.
- The body, when there is one, explains why, in lines of 100 characters at most.
- A breaking change, typically a new packet format, takes a `!` after the type and a `BREAKING CHANGE:` line in the body.
- No Linear identifier, issue or task number in a commit: the link to the issue lives in the pull request.
- The message stops at its content: no `Co-Authored-By` trailer, and no mention of a tool that helped write the change.

Before each commit, read what is staged: `git diff --cached --name-status` must list exactly the files the message announces, and never a build output (`build/`, the binaries at the root, `compile_commands.json`).

## Code style

`clang-format` (Google style, 2-space indent, 80 columns) and `clang-tidy` (every warning an error) are configured at the repository root. Format before committing, and lint the `.cpp` and `.hpp` files you changed, headers included: the CI lints each of them.

```bash
cmake --build build --target format
cmake --build build --target tidy
```

On macOS, the `tidy` target does not find the system headers, and missing headers hide findings. Run `clang-tidy` on the changed files with the SDK instead, and trust the result only if it exits with 0 and prints no `Error while processing` line, which would mean a file was analyzed only in part:

```bash
SDK=$(xcrun --show-sdk-path)
clang-tidy -p build --extra-arg=-isysroot --extra-arg="$SDK" \
  --extra-arg=-isystem --extra-arg="$SDK/usr/include/c++/v1" <changed files>
```

### Names

| What | Rule | Example |
|---|---|---|
| Files | the exact `PascalCase` name of the class they declare; a test file adds `Test` | `EntityRegistry.hpp`, `WaveDirectorTest.cpp` |
| Interfaces | an `I` prefix | `IClock` |
| Private and protected members | a trailing underscore, never a prefix | `socket_`, not `m_socket` |
| Public members of a struct | bare names | `assetId` and `rectangle` in `TextureArea` |
| Constants | no `k` prefix; `UPPER_SNAKE_CASE` when a header shares them | `DEFAULT_SERVER_PORT` |
| `enum class` values | `PascalCase` | `EntityType::PlayerMissile` |
| Everything | full words, never an abbreviation; `id`, `udp` and `ui` count as words | `position`, not `pos`; `clientId` |
| Types | named after the data they hold, without a catch-all suffix (`Manager`, `Helper`, `Utils`, `Data`) | `EntityRegistry`, not `EntityManager` |

### Comments

- English only, above declarations in the `.hpp`, as Doxygen (`/** @brief … */`). A comment says the why the code cannot say, never the what.
- No comment inside a function body: a step that needs one needs a name, so extract a function or rename a variable. Tool directives (`// NOLINTNEXTLINE`, `// clang-format off`) are the exception.
- No ticket identifier, no `TODO`, no commented-out code.

## Code structure

| Rule | In short |
|---|---|
| Headers declare, sources define | No function body in a `.hpp`, one-line getters included. Only templates, `constexpr` and `= default` or `= delete` members stay there. |
| A module is one folder and one library | A module groups the classes of one feature (`src/server/Spawning/`). Its `CMakeLists.txt` declares one library, `rtype_<owner>_<module>`, listing the `.cpp` of every class; each class is a `.hpp` and `.cpp` pair of the folder, never a folder of its own. A module holds at most 10 `.cpp`. A sub-folder with a library of its own, a sub-module, exists only for a group that needs a dependency the rest of the module must not link, that another module reuses, or when the module would pass ten. Two classes of a module never include each other. A module written before this layout keeps a folder per class until it is flattened in a `refactor` commit of its own. See [Adding a class](https://nique-epi.github.io/R-Type/project-layout/#adding-a-class). |
| One `CMakeLists.txt` per folder | It declares the library of its folder (the one library of a module) and calls `rtype_enable_warnings()`; the parent adds it with `add_subdirectory`. |
| Who may link what | The server and the shared libraries never link SFML; the configure stops if they do. See [Who may link what](https://nique-epi.github.io/R-Type/project-layout/#who-may-link-what). |
| Interfaces are pure | An interface holds only `= 0` methods and a `= default` virtual destructor: no body, no data. |
| Inherit interfaces, compose the rest | A class derives only from interfaces. To reuse code, it holds an object as a member. No `protected` data. |
| No type inside a class | No nested `struct`, `enum` or `using` alias: each type gets a header of its own, in the module folder next to the class that uses it. |
| At most 10 member functions | Constructors, the destructor and `= default` members are not counted. A larger class is split along the state its functions share. |
| At most 3 parameters | Pass the whole object, or a struct named after what the values are, built with designated initializers: `TextureArea{.assetId = …, .rectangle = …}`. |
| One kind of step per function | Checks that decide, writes that build, and notifications (a sound, a message, a log) go in separate functions. |
| No magic value | Every meaningful literal is a named constant in a `<Concept>Constants.hpp`, which holds constants only. `0`, `1`, `nullptr` and `true` are not magic. |
| Errors are the module's exceptions | Each module has an `Exceptions/` folder with a root exception and a subclass per case. Never `throw std::runtime_error` or another standard exception directly. |
| Designated initializers | An aggregate of two members or more is built with `Type{.first = a, .second = b}`. |

A file you touch that already breaks one of these rules is fixed, not extended: in a `refactor` commit of its own when the fix stays in the file or the module, or reported in the pull request and discussed first when it would spread across modules.

### Writing a module

A module that implements `IModule` is installed, started and stopped in the order of its program's list:

- `install()` prepares and checks, and keeps nothing open: it may read a file and close it, build the objects the module owns, or throw on a wrong configuration.
- `start()` opens what must live until `stop()`: a socket, a thread, an open file, an event subscription, a timer. A `start()` that opens several things releases the ones already open before an exception leaves it, because `Application` never calls `stop()` on the module whose `start()` threw.
- `stop()` closes everything `start()` opened, in the reverse order. Test the failure of a second step as well as the success: the first resource must be released.

## Tests

Tests use GoogleTest. Each test executable is declared with `rtype_add_test()` in `tests/CMakeLists.txt`, and `tests/` mirrors `src/`: see [Project layout](https://nique-epi.github.io/R-Type/project-layout/#tests-web-frontend-assets).

Each test checks one behavior and says which in a Given / When / Then comment above it:

```cpp
/**
 * Given a new wave director on the test curve
 * When its wave is asked for
 * Then it names the first wave of the curve
 */
TEST_F(WaveDirectorFixture, StartsWithTheFirstWave) {
  EXPECT_EQ(director().waveNumber(), FIRST_WAVE_NUMBER_IN_THE_LOG);
  EXPECT_EQ(director().currentWave(), TEST_WAVE_CURVE.firstWave);
}
```

- Suite and test names are `PascalCase` and describe the behavior. Two independent `Then`s are two tests.
- A `…Test.cpp` holds only its includes, its `using` lines and its tests. Its constants go in `<Subject>TestConstants.hpp`, its fixture in `<Subject>Fixture.{hpp,cpp}` and its helper functions in `<Subject>Fixtures.{hpp,cpp}`, next to it. A class that stands in for a production one goes in `tests/doubles/`.
- An assertion states what the product requires, never what the code happened to return: that decoding an encoded message gives the message back, rather than the byte count the encoder produced on the day the test was written. When a requirement fixes a value, the test writes that value, never the constant the code under test reads.
- A test double keeps the risky properties of what it replaces: a fake network loses, repeats and reorders datagrams; a fake clock moves.
- A test added to an existing test executable links what it uses: a gmock matcher needs `GTest::gmock`, and a header of `src/` needs the library that exposes it. Build that executable alone right after adding the test.
- Watch a new test fail on the faulty code before keeping it: a test that was never red proves nothing.

```bash
cmake --workflow --preset test
```

It configures in Debug, builds and runs every test. It must pass with zero warnings, counted on a build that recompiled the files you changed: an incremental rerun that compiles nothing shows no warning either.

## Code that reads the network

The server reads datagrams from clients it cannot trust:

- every size, index or identifier read from a datagram is bounded before it is used, and a datagram that is truncated, too long or of an unknown type is dropped without a crash; a bound is tested on the empty input, on the full input and one step on each side of it;
- the wire format is explicit, with fixed sizes and a byte order (see [Protocol](https://nique-epi.github.io/R-Type/protocol/)), never a struct copied as is;
- a client can never make the server allocate memory in proportion to a value it sends;
- a check runs before the write it protects, never after;
- an exception to a check, such as a message accepted from an unknown address, is first played through as an attacker who forges the address and repeats the message every second;
- data shared between the network thread and the game loop goes through a queue (see [Concurrency](https://nique-epi.github.io/R-Type/concurrency/)).

## Documentation

A change that adds or changes something another team uses (a class, a named constant, a folder, a protocol message, a command-line option, a build step) updates the site and the README in the same pull request, in a `docs:` commit. Search every page for what the change replaced, not only the pages it is about:

```bash
git grep -n -i -e '<old name>' -- docs/src/content README.md
```

`npm run build` in `docs/` must succeed: it fails on a broken internal link, as the documentation check does. The README's Documentation section shows how to preview the site.

## Pull requests

Open the pull request as a draft, with a title written like a commit message. The title and the description are in English; the comments posted on the pull request (replies, review comments, follow-ups) are in French. Its description has these sections, in this order:

| Section | Holds |
|---|---|
| `## Summary` | What the change does and why, in a few lines, then `Closes RTY-<n>` for its Linear issue, or `Part of RTY-<n>` when it delivers only part of it. A pull request stacked on another one says so first. |
| `## Changes` | One line per file or area changed. |
| `## Type of Change` | The boxes `Bug fix`, `New feature`, `Refactor` and `Documentation`, each ticked only if true. |
| `## Testing` | The commands you ran, on which system and compiler, and their result. |
| `## Checklist` | `Code compiles without errors`, `No new warnings introduced`, `Tests pass` and `Self-reviewed the diff`, each ticked only if verified. |

Linear links the pull request only when the issue identifier follows a closing word (`Closes`, `Fixes`, `Resolves`) or `Part of` in the description: a mention such as `Covers RTY-<n>` links nothing. After opening the pull request, check that the issue lists it.

Then follow its checks until they are green:

| Check | Runs |
|---|---|
| Format check | `clang-format` 20 on every `.cpp` and `.hpp` of `src/` and `tests/` |
| Clang-tidy (1/4 to 4/4) | `clang-tidy` 22 on the `.cpp` and `.hpp` files the pull request changes, or on every file when it changes `.clang-tidy` or the workflow |
| Clang-tidy (Windows-only files) | the `…Windows.cpp` files, which only Windows compiles |
| Build and test (ubuntu-latest, windows-latest) | the `test` preset with warnings as errors, under GCC on Linux and MSVC on Windows, then every test |
| Build documentation | `npm ci` and `npm run build` in `docs/` |

The CI builds the pull request merged with the current `main`. When `main` has moved, bring it into your branch and build again before pushing: a name your branch renamed may still be used by code merged since.

Mark the pull request ready for review once you have reread its whole diff and its checks are green.

## Review

A review is done by someone who did not write the code. Each finding gives its place (`file:line`), a concrete failure (the input or the state, then the wrong result) and one severity:

| Severity | Means | Example |
|---|---|---|
| Critical | a crash, undefined behavior, corrupted state, or a flaw a remote client can use | an out-of-bounds read on a received packet |
| Warning | a bug, a slowdown, or a rule of this guide broken | a function body in a `.hpp`, an allocation every frame |
| Info | style or readability; a matter of taste starts with `Nit:` | a clearer name |

When unsure between two severities, the reviewer picks the lower one. The verdict is `NO-GO` when there is a Critical, `GO-with-changes` when Warnings must be fixed before the merge, and `GO` otherwise.

The review reads, in order: the design (the right library, dependencies pointing the right way), the behavior on Linux and Windows, the complexity (code for a future that may never come is a finding), whether the tests would fail if the code broke, the names, the comments, and the cost of the code that runs every frame or every tick.

## Definition of done

A change is done when:

- it sits on its own branch, started from `origin/main`, with one intent per commit;
- `cmake --workflow --preset test` passes with zero warnings;
- the format check and `clang-tidy` pass on the files it changes;
- once its plan was applied, the changed code was reviewed for reuse and simplification (`/simplify`) before the final checks above were run;
- it breaks none of the rules of [Code style](#code-style) and [Code structure](#code-structure);
- each behavior it adds or fixes has a Given / When / Then test;
- the site and the README describe it;
- its commits and the title and description of its pull request are in English, its pull request comments are in French, none of them mentions the tools used to write them, and the pull request was opened as a draft.
