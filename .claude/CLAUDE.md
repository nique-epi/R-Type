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

Every file in `.claude/rules/` (subdirectories included) is an isolated rule, loaded automatically. Applying them is not optional: a violated rule is a defect, not a preference. On a mistake in the code, `core/errors-learning.md` requires creating a new one; a mistake outside the code (shell, git, local machine, tools, conversation) produces none.

Paths below are relative to `.claude/rules/`.

- **Without `paths`** (`core/`, `fix/`): loaded every session.
- **With `paths: ["**/*.{cpp,hpp,tpp}"]`** (`architecture/`, `code-style/`): loaded only when a C++ file is read. **Before writing C++ without having read a C++ file in the session** (new file, new module), read these two folders explicitly.

### Entry points by situation

| Situation | Apply |
|---|---|
| Writing C++ | `architecture/`, `code-style/` |
| Writing a test | `code-style/code-style-tests-given-when-then.md`, `code-style/fix-format-a-test-file-holds-only-tests.md`, `fix/process/fix-process-green-test-locking-a-defect-and-overly-deterministic-fake.md` |
| Writing a module (`IModule`) | `architecture/fix-architecture-a-module-install-keeps-nothing-open.md`, `architecture/fix-architecture-a-module-start-that-throws-closes-what-it-opened.md` |
| Touching the network | security section of `core/code-review.md`, `fix/reasoning/fix-reasoning-an-exception-to-a-check-is-read-as-the-forger.md`, `fix/reasoning/fix-reasoning-last-received-is-not-newest-over-udp.md` |
| Creating a branch | `fix/process/fix-process-branch-before-first-commit.md`, `fix/execution/fix-execution-checkout-b-starts-from-head-not-main.md`, `fix/process/fix-process-reread-rules-after-switching-branch.md` |
| Finishing the application of a plan | `core/simplify-after-plan.md` — `/simplify` before the final gates |
| Committing | `core/commit.md`, `fix/process/fix-process-no-co-author-trailer.md`, `fix/execution/fix-execution-check-the-index-before-committing.md` |
| Finishing a feature | the `document-feature` skill (`.claude/skills/document-feature/SKILL.md`), `fix/process/fix-process-grep-every-page-for-what-the-feature-replaced.md` |
| Opening a PR | `fix/format/fix-format-pull-requests-in-english.md`, `fix/format/fix-format-pr-links-its-linear-issue.md`, `fix/process/fix-process-pr-always-opened-as-draft.md`, the `document-feature` skill if the feature is not documented yet |
| Commenting on a PR, answering the user | `fix/format/fix-format-french-in-the-conversation-and-pr-comments.md` |
| Reviewing | `core/code-review.md` |
| Announcing a green gate | `fix/execution/fix-execution-zsh-pipestatus.md`, `fix/execution/fix-execution-zero-warnings-proven-on-a-recompiling-build.md`, `fix/execution/fix-execution-run-clang-tidy-before-push.md` |
| Writing a documentation page or a diagram | `fix/format/fix-format-schematic-diagrams-and-map-level-pages.md`, `fix/format/fix-format-context-before-the-deliverable.md`, the `document-feature` skill |
| Making a mistake | `core/errors-learning.md` — a mistake in the code produces a new rule, a mistake outside it none |
| Pausing, finishing a step | `core/context-continuity.md` |

### Index

**Contract — `core/` (loaded every session)**
- `core/code-review.md`
- `core/commit.md`
- `core/context-continuity.md`
- `core/errors-learning.md`
- `core/simplify-after-plan.md`

**C++ architecture — `architecture/` (loaded when a C++ file is read)**
- `architecture/architecture-a-module-is-one-folder-and-one-target.md`
- `architecture/architecture-custom-exceptions-per-module.md`
- `architecture/architecture-hpp-declarations-cpp-definitions.md`
- `architecture/architecture-pure-virtual-interfaces.md`
- `architecture/fix-architecture-a-function-does-one-kind-of-step.md`
- `architecture/fix-architecture-a-module-install-keeps-nothing-open.md`
- `architecture/fix-architecture-a-module-start-that-throws-closes-what-it-opened.md`
- `architecture/fix-architecture-at-most-ten-methods-per-class.md`
- `architecture/fix-architecture-at-most-three-parameters.md`
- `architecture/fix-architecture-inherit-interfaces-compose-implementations.md`
- `architecture/fix-architecture-magic-values-as-named-constants.md`
- `architecture/fix-architecture-no-type-nested-in-a-class.md`

**Build architecture — `fix/architecture/` (loaded every session)**
- `fix/architecture/fix-architecture-a-link-never-points-back-at-a-folder-that-links-it.md`
- `fix/architecture/fix-architecture-one-cmakelists-per-folder.md`

**Code style — `code-style/` (loaded when a C++ file is read)**
- `code-style/code-style-comments-english-outside-bodies-no-tickets.md`
- `code-style/code-style-constants-without-k-prefix.md`
- `code-style/code-style-file-named-after-its-class.md`
- `code-style/code-style-full-names-no-abbreviations.md`
- `code-style/code-style-private-members-trailing-underscore.md`
- `code-style/code-style-tests-given-when-then.md`
- `code-style/fix-format-a-constants-header-ends-in-constants.md`
- `code-style/fix-format-a-test-file-holds-only-tests.md`
- `code-style/fix-format-designated-initializers-for-aggregates.md`
- `code-style/fix-format-name-after-data-not-process.md`
- `code-style/fix-reasoning-a-pointer-into-a-temporary-dies-with-it.md`

**Process — `fix/process/`**
- `fix/process/fix-process-a-model-is-called-not-copied.md`
- `fix/process/fix-process-a-small-feature-is-checked-in-proportion.md`
- `fix/process/fix-process-a-screen-without-mockup-is-validated-as-a-picture.md`
- `fix/process/fix-process-ask-what-is-observed-before-building-a-protocol.md`
- `fix/process/fix-process-branch-before-first-commit.md`
- `fix/process/fix-process-defect-injection-on-an-isolated-copy.md`
- `fix/process/fix-process-do-not-over-engineer-step-back.md`
- `fix/process/fix-process-follow-ci-after-opening-a-pr.md`
- `fix/process/fix-process-green-test-locking-a-defect-and-overly-deterministic-fake.md`
- `fix/process/fix-process-grep-every-page-for-what-the-feature-replaced.md`
- `fix/process/fix-process-instrument-before-reasoning-docs-are-truth.md`
- `fix/process/fix-process-no-co-author-trailer.md`
- `fix/process/fix-process-plan-mode-writes-only-the-plan.md`
- `fix/process/fix-process-pr-always-opened-as-draft.md`
- `fix/process/fix-process-read-the-project-map-before-placing-a-folder.md`
- `fix/process/fix-process-relay-agent-report-separating-verified-facts.md`
- `fix/process/fix-process-reread-rules-after-switching-branch.md`
- `fix/process/fix-process-resolve-mechanical-conflicts-escalate-big-ones.md`
- `fix/process/fix-process-rework-preexisting-inconsistencies.md`
- `fix/process/fix-process-verification-tooling-stays-on-my-side.md`
- `fix/process/fix-process-visible-defect-never-minor-on-my-own.md`

**Reasoning — `fix/reasoning/`**
- `fix/reasoning/fix-reasoning-a-guard-after-the-write-is-only-an-alarm.md`
- `fix/reasoning/fix-reasoning-a-sanitizer-sees-only-what-leaves-the-allocation.md`
- `fix/reasoning/fix-reasoning-a-sequence-asserts-its-starting-state.md`
- `fix/reasoning/fix-reasoning-an-error-type-is-not-a-diagnosis.md`
- `fix/reasoning/fix-reasoning-an-exception-to-a-check-is-read-as-the-forger.md`
- `fix/reasoning/fix-reasoning-bounded-contract-check-the-empty-state.md`
- `fix/reasoning/fix-reasoning-check-code-state-before-scoping.md`
- `fix/reasoning/fix-reasoning-check-the-delivered-artifact-not-its-footprint.md`
- `fix/reasoning/fix-reasoning-convention-from-a-single-sample.md`
- `fix/reasoning/fix-reasoning-count-a-delay-on-the-timeline.md`
- `fix/reasoning/fix-reasoning-delegated-invariant-must-be-computed.md`
- `fix/reasoning/fix-reasoning-evidence-that-does-not-discriminate.md`
- `fix/reasoning/fix-reasoning-last-received-is-not-newest-over-udp.md`
- `fix/reasoning/fix-reasoning-name-the-axis-when-neutral-here-beneficial-there.md`
- `fix/reasoning/fix-reasoning-never-assume-unread-file-content.md`
- `fix/reasoning/fix-reasoning-present-code-is-not-behavior.md`
- `fix/reasoning/fix-reasoning-recheck-a-conclusion-after-the-design-changed.md`
- `fix/reasoning/fix-reasoning-use-measurements-to-falsify-before-coding.md`
- `fix/reasoning/fix-reasoning-validate-the-instrument-before-reporting-a-count.md`
- `fix/reasoning/fix-reasoning-verify-the-premise-before-fixing-downstream.md`

**Execution (shell, git, build) — `fix/execution/`**
- `fix/execution/fix-execution-a-script-reading-repository-text-names-its-encoding.md`
- `fix/execution/fix-execution-a-split-commit-carries-its-own-links.md`
- `fix/execution/fix-execution-a-static-library-reached-twice-on-the-link-line.md`
- `fix/execution/fix-execution-an-edit-of-a-shared-text-checks-it-is-unchanged.md`
- `fix/execution/fix-execution-browser-global-names-are-window-properties.md`
- `fix/execution/fix-execution-check-the-index-before-committing.md`
- `fix/execution/fix-execution-checkout-b-starts-from-head-not-main.md`
- `fix/execution/fix-execution-clang-format-only-on-cpp-files.md`
- `fix/execution/fix-execution-edit-script-validates-anchors-before-writing.md`
- `fix/execution/fix-execution-every-build-runs-in-parallel.md`
- `fix/execution/fix-execution-git-check-branch-before-writing-history.md`
- `fix/execution/fix-execution-gcc-warns-when-a-designated-initializer-skips-a-member-without-default.md`
- `fix/execution/fix-execution-git-stash-pop-without-effective-push.md`
- `fix/execution/fix-execution-new-caller-keeps-existing-callers-guards.md`
- `fix/execution/fix-execution-run-clang-tidy-before-push.md`
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
- `fix/format/fix-format-a-count-on-screen-names-what-it-counts.md`
- `fix/format/fix-format-a-lost-unit-looks-the-same-at-every-count.md`
- `fix/format/fix-format-a-summary-keeps-the-conditions-of-its-rule.md`
- `fix/format/fix-format-context-before-the-deliverable.md`
- `fix/format/fix-format-french-in-the-conversation-and-pr-comments.md`
- `fix/format/fix-format-no-tool-call-artifacts-in-written-files.md`
- `fix/format/fix-format-plan-shows-all-code-and-explains-it.md`
- `fix/format/fix-format-pr-links-its-linear-issue.md`
- `fix/format/fix-format-pull-requests-in-english.md`
- `fix/format/fix-format-schematic-diagrams-and-map-level-pages.md`

**Interpretation — `fix/interpretation/`**
- `fix/interpretation/fix-interpretation-confirm-ambiguous-negation-before-inverting.md`
- `fix/interpretation/fix-interpretation-flagged-placeholder-must-be-replaced.md`
- `fix/interpretation/fix-interpretation-positive-guideline-is-not-a-prohibition.md`
- `fix/interpretation/fix-interpretation-typo-reading-that-contradicts-a-rule.md`

### Skills

Skills live in `.claude/skills/<name>/SKILL.md`, relative to the repository root. Each one is a procedure loaded on demand, not a rule.

- `document-feature`: writes or updates the documentation site (`docs/src/content/docs/`) and the README for a finished feature, in the house style of the existing pages.

## Definition of done

- [ ] Dedicated branch started from `origin/main`, one intent per commit.
- [ ] `/simplify` run once the plan is fully applied, before the gates below.
- [ ] `cmake --workflow --preset test` green, **zero warnings**, output and exit code read.
- [ ] `format-check` and `tidy` green.
- [ ] No violation of the `architecture/` and `code-style/` rules.
- [ ] Given / When / Then tests for the added or fixed behavior.
- [ ] Commits, PR title and description in English, PR comments in French, **no AI mention**, PR opened as a draft.
