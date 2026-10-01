---
description: Comments — English, Doxygen above declarations, no comment inside function bodies, zero ticket reference
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: Comments in English, above declarations, never inside bodies, no ticket references

## Goal

A comment inside a function describes what the nearby code does — which the code already says when names are good (see `code-style-full-names-no-abbreviations.md`). When the code changes, the comment rots and ends up lying. Doxygen documentation above declarations is picked up by tooling (clangd, IDEs) and describes the **contract**, which changes much less than the implementation.

## Rules

### Where

- **No comment inside a function, method or lambda body**: no `//`, no `/* */`, no parameter hint (`/*isReliable=*/true`).
- A step that needs an explanation actually needs a **name**: extract a function or rename a variable.
- What applies to the whole body (pitfall, invariant, required order) moves up into the function's doc comment.
- Documentation lives **above declarations**, in the `.hpp`, as **Doxygen** (`/** @brief … @param … @returns … */`). Document types and non-trivial public members; no doc that repeats the signature.

### Exceptions

- Tooling directives: `// NOLINTNEXTLINE(check)`, `// NOLINTBEGIN` / `// NOLINTEND`, `// clang-format off` / `on`.
- File header required by the Epitech norm, if required.
- Doxygen above an internal helper (anonymous namespace) reused in the file whose contract deserves to be written.

### What

- **English only.**
- The non-obvious **why** (constraint, invariant), never the **what**.
- **Zero reference to planning artifacts** — no ticket, issue, task or review-finding id — in comments, variable names, test names or messages. An invariant is explained in plain words, without the id.
- No decorative separators, no orphan `TODO`, no commented-out code.

## Examples

- ❌ **Forbidden**:
  ```cpp
  void MovementSystem::update(Registry &registry, float deltaTime) {
    // Move every entity that has a position and a velocity (TASK-14)
    for (auto [entity, position, velocity] : registry.view<Position, Velocity>()) {
      // apply the velocity
      position.x += velocity.x * deltaTime;
      position.y += velocity.y * deltaTime;
    }
  }

  send(packet, /*isReliable=*/true);
  ```
- ✅ **Instead**:
  ```cpp
  /**
   * @brief Advances every entity that has both a Position and a Velocity.
   *
   * @param[in,out] registry  Registry holding the components to update.
   * @param[in]     deltaTime Elapsed time since the previous tick, in seconds.
   */
  void update(Registry &registry, float deltaTime) override;
  ```
  ```cpp
  void MovementSystem::update(Registry &registry, float deltaTime) {
    for (auto [entity, position, velocity] : registry.view<Position, Velocity>()) {
      position.x += velocity.x * deltaTime;
      position.y += velocity.y * deltaTime;
    }
  }

  send(packet, Delivery::Reliable);
  ```
