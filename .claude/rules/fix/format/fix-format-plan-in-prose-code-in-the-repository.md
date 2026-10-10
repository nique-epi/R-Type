---
description: "An implementation plan is written in prose (decisions, files, behavior, tests, on-screen checks); code goes in the repository at implementation, never in the chat"
trigger: always_on
---

# RULE: A plan is written in prose and leaves the on-screen check to the user; code goes in the repository, never in the chat

Why: the user validates the design, the files and the behavior; code pasted in the chat costs context and credits for nothing.

## Rule

1. **No code file in the chat**, plans included: no header, source, CMake file or test pasted in an answer. A one-line snippet only when a question cannot be answered without it.
2. **A plan holds, in prose**: the context, the decisions with their reason, the files to create or change with one line each, the resulting behavior, the tests by name with what each proves, the risks.
3. **Each decision or file explained follows this order**: Why (the need, in plain words), What we do, What it does.
4. **The on-screen check is a procedure for the user**: a table action, expected on screen, expected in the logs. The agent keeps the build, `format-check`, `tidy` and the unit tests. Exception: the user explicitly asks the agent to run and observe the game.
5. **An API behavior the design relies on is cited with its source** (headers installed by vcpkg, official documentation of the pinned version).
6. **The code is written in the repository at implementation**, once the plan is validated; the user reviews it in the diff.

### A snippet went through the tools, and a prediction is called a prediction

1. **A snippet shown in a plan (point 1) is formatted by `clang-format`, not wrapped by hand**: write it to a scratch file, run `clang-format --assume-filename=<its future path>` on it, and paste the result. Say which `clang-format` produced it (`clang-format --version`), and whether it is the version the CI runs.
2. **"These tests will fail without the fix" is a prediction until it is run.** Write it as one in the plan, then report the measured list, including the tests that did not behave as predicted and why.

## Example

- ❌ **Before (wrong)**: a plan that pastes the full content of nine files in the chat.
- ✅ **After (right)**: "`AssetLibrary` (new, `src/client/Assets/AssetLibrary/`): loads every asset at launch, then finds it by id without reading the disk; tested by `LookUpsNeverReadTheDisk`", the table of checks the user runs in the game, and the code written later in the repository.
