# CLAUDE.md — R-Type

Instructions for agents working on this repository. They complement each developer's global `CLAUDE.md`; they never relax it.

## The project

Networked multiplayer remake of the R-Type shoot'em up, on a custom C++ game engine. Epitech tek3 team project.

| Item | Choice |
|---|---|
| Language | C++20, compiled by GCC/Clang (Linux) and MSVC (Windows) |
| Build | CMake ≥ 3.28, presets (`default`, `test`), dependencies through vcpkg (pinned submodule) |
| Libraries | SFML 3 (`graphics`, `window`, `audio`) on the client, standalone Asio on the server, GoogleTest for tests |
| Targets | `r-type_server` (Asio only, never SFML), `r-type_client` (SFML), `harness_tests` |
| Style | `.clang-format` (Google base, 2-space indent, 80 columns), `.clang-tidy` (warnings = errors) |

The `src/server`, `src/client`, `tests` layout is provisional: the engine/game organization is not decided yet. Read the repository before assuming a structure.

## Commands

```bash
git submodule update --init
cmake --workflow --preset build                 # Release, binaries at the repository root
cmake --workflow --preset test                  # Debug + tests, MUST be green with zero warnings
cmake --build build --target format-check       # formatting
cmake --build build --target format             # format in place
cmake --build build --target tidy               # clang-tidy
```

The CI (`build-and-test`) checks formatting, then builds and tests on Linux (GCC) and Windows (MSVC), warnings treated as errors.

## Rules

Every file in `.claude/rules/` (subdirectories included) is an isolated rule, loaded automatically. Applying them is not optional: a violated rule is a defect, not a preference. On any mistake, `core/errors-learning.md` requires creating a new one.

Paths below are relative to `.claude/rules/`.

- **Without `paths`** (`core/`, `fix/`): loaded every session.
- **With `paths: ["**/*.{cpp,hpp,tpp}"]`** (`architecture/`, `code-style/`): loaded only when a C++ file is read. **Before writing C++ without having read a C++ file in the session** (new file, new module), read these two folders explicitly.

### Entry points by situation

| Situation | Apply |
|---|---|
| Writing C++ | `architecture/`, `code-style/` |
| Writing a test | `code-style/code-style-tests-given-when-then.md`, `fix/process/fix-process-green-test-locking-a-defect-and-overly-deterministic-fake.md` |
| Touching the network | security section of `core/code-review.md` |
| Creating a branch | `fix/process/fix-process-branch-before-first-commit.md`, `fix/execution/fix-execution-checkout-b-starts-from-head-not-main.md` |
| Committing | `core/commit.md`, `fix/process/fix-process-no-co-author-trailer.md`, `fix/execution/fix-execution-check-the-index-before-committing.md` |
| Opening a PR | `fix/format/fix-format-pull-requests-in-english.md`, `fix/process/fix-process-pr-always-opened-as-draft.md` |
| Reviewing | `core/code-review.md` |
| Announcing a green gate | `fix/execution/fix-execution-zsh-pipestatus.md`, `fix/execution/fix-execution-zero-warnings-proven-on-a-recompiling-build.md` |
| Making a mistake | `core/errors-learning.md` — the mistake produces a new rule |
| Pausing, finishing a step | `core/context-continuity.md` |

### Index

**Contract — `core/` (loaded every session)**
- `core/code-review.md`
- `core/commit.md`
- `core/context-continuity.md`
- `core/errors-learning.md`

**C++ architecture — `architecture/` (loaded when a C++ file is read)**
- `architecture/architecture-custom-exceptions-per-module.md`
- `architecture/architecture-hpp-declarations-cpp-definitions.md`
- `architecture/architecture-pure-virtual-interfaces.md`
- `architecture/fix-architecture-magic-values-as-named-constants.md`

**Code style — `code-style/` (loaded when a C++ file is read)**
- `code-style/code-style-comments-english-outside-bodies-no-tickets.md`
- `code-style/code-style-constants-without-k-prefix.md`
- `code-style/code-style-file-named-after-its-class.md`
- `code-style/code-style-full-names-no-abbreviations.md`
- `code-style/code-style-private-members-trailing-underscore.md`
- `code-style/code-style-tests-given-when-then.md`
- `code-style/fix-format-name-after-data-not-process.md`

**Process — `fix/process/`**
- `fix/process/fix-process-ask-what-is-observed-before-building-a-protocol.md`
- `fix/process/fix-process-branch-before-first-commit.md`
- `fix/process/fix-process-do-not-over-engineer-step-back.md`
- `fix/process/fix-process-green-test-locking-a-defect-and-overly-deterministic-fake.md`
- `fix/process/fix-process-instrument-before-reasoning-docs-are-truth.md`
- `fix/process/fix-process-no-co-author-trailer.md`
- `fix/process/fix-process-pr-always-opened-as-draft.md`
- `fix/process/fix-process-relay-agent-report-separating-verified-facts.md`
- `fix/process/fix-process-resolve-mechanical-conflicts-escalate-big-ones.md`
- `fix/process/fix-process-rework-preexisting-inconsistencies.md`
- `fix/process/fix-process-visible-defect-never-minor-on-my-own.md`

**Reasoning — `fix/reasoning/`**
- `fix/reasoning/fix-reasoning-a-guard-after-the-write-is-only-an-alarm.md`
- `fix/reasoning/fix-reasoning-a-sequence-asserts-its-starting-state.md`
- `fix/reasoning/fix-reasoning-an-error-type-is-not-a-diagnosis.md`
- `fix/reasoning/fix-reasoning-bounded-contract-check-the-empty-state.md`
- `fix/reasoning/fix-reasoning-check-code-state-before-scoping.md`
- `fix/reasoning/fix-reasoning-check-the-delivered-artifact-not-its-footprint.md`
- `fix/reasoning/fix-reasoning-convention-from-a-single-sample.md`
- `fix/reasoning/fix-reasoning-delegated-invariant-must-be-computed.md`
- `fix/reasoning/fix-reasoning-evidence-that-does-not-discriminate.md`
- `fix/reasoning/fix-reasoning-name-the-axis-when-neutral-here-beneficial-there.md`
- `fix/reasoning/fix-reasoning-never-assume-unread-file-content.md`
- `fix/reasoning/fix-reasoning-present-code-is-not-behavior.md`
- `fix/reasoning/fix-reasoning-use-measurements-to-falsify-before-coding.md`
- `fix/reasoning/fix-reasoning-validate-the-instrument-before-reporting-a-count.md`
- `fix/reasoning/fix-reasoning-verify-the-premise-before-fixing-downstream.md`

**Execution (shell, git, build) — `fix/execution/`**
- `fix/execution/fix-execution-check-the-index-before-committing.md`
- `fix/execution/fix-execution-checkout-b-starts-from-head-not-main.md`
- `fix/execution/fix-execution-edit-script-validates-anchors-before-writing.md`
- `fix/execution/fix-execution-git-check-branch-before-writing-history.md`
- `fix/execution/fix-execution-git-stash-pop-without-effective-push.md`
- `fix/execution/fix-execution-new-caller-keeps-existing-callers-guards.md`
- `fix/execution/fix-execution-silent-push-and-detached-head-compare-remote-sha.md`
- `fix/execution/fix-execution-touch-recreates-deleted-files.md`
- `fix/execution/fix-execution-unquoted-heredoc-runs-backticks.md`
- `fix/execution/fix-execution-validate-against-the-target-version.md`
- `fix/execution/fix-execution-zero-warnings-proven-on-a-recompiling-build.md`
- `fix/execution/fix-execution-zsh-lowercase-path-overwrites-PATH.md`
- `fix/execution/fix-execution-zsh-pipestatus.md`
- `fix/execution/fix-execution-zsh-unquoted-special-characters.md`
- `fix/execution/fix-execution-zsh-variable-colon-modifiers.md`
- `fix/execution/fix-execution-zsh-word-splitting.md`

**Format — `fix/format/`**
- `fix/format/fix-format-context-before-the-deliverable.md`
- `fix/format/fix-format-no-tool-call-artifacts-in-written-files.md`
- `fix/format/fix-format-pull-requests-in-english.md`

**Interpretation — `fix/interpretation/`**
- `fix/interpretation/fix-interpretation-confirm-ambiguous-negation-before-inverting.md`
- `fix/interpretation/fix-interpretation-flagged-placeholder-must-be-replaced.md`
- `fix/interpretation/fix-interpretation-positive-guideline-is-not-a-prohibition.md`
- `fix/interpretation/fix-interpretation-typo-reading-that-contradicts-a-rule.md`

## Definition of done

- [ ] Dedicated branch started from `origin/main`, one intent per commit.
- [ ] `cmake --workflow --preset test` green, **zero warnings**, output and exit code read.
- [ ] `format-check` and `tidy` green.
- [ ] No violation of the `architecture/` and `code-style/` rules.
- [ ] Given / When / Then tests for the added or fixed behavior.
- [ ] Commits and PR in English, **no AI mention**, PR opened as a draft.
