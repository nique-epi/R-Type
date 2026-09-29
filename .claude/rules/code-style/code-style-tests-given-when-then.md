---
description: GoogleTest tests — one behavior per test, descriptive name, Given / When / Then doc comment above the TEST
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: Every test describes its scenario as Given / When / Then

## Goal

A test name says *what*, not *under which conditions* nor *what is expected*. Given / When / Then makes the initial state, the action and the result explicit: it acts as a mini-specification, speeds up review and failure diagnosis, and pushes toward validating **a single** behavior per test.

## Rules

- Every `TEST` / `TEST_F` / `TEST_P` has, **above** its declaration, a three-clause doc comment, one per line: `Given` (initial context), `When` (action), `Then` (expected result). It sits above, not in the body (see `code-style-comments-english-outside-bodies-no-tickets.md`).
- **One behavior per test.** Two independent `Then`s = two tests.
- Suite and test names in `PascalCase`, describing the behavior: `TEST(PacketReader, RejectsPayloadShorterThanHeader)`. No ticket id.
- The `Then` states what the product requires, never the observed output (see `fix-process-green-test-locking-a-defect-and-overly-deterministic-fake.md`).
- Tests are declared through `rtype_add_test()` in `tests/CMakeLists.txt`.

## Examples

- ❌ **Forbidden**:
  ```cpp
  TEST(Registry, Test1) {
    Registry registry;
    auto entity = registry.createEntity();
    registry.destroyEntity(entity);
    EXPECT_FALSE(registry.isAlive(entity));
    EXPECT_EQ(registry.size(), 0);
  }
  ```
- ✅ **Instead**:
  ```cpp
  /**
   * Given a registry holding a single entity
   * When that entity is destroyed
   * Then the registry no longer reports it as alive
   */
  TEST(Registry, DestroyedEntityIsNoLongerAlive) {
    Registry registry;
    auto entity = registry.createEntity();

    registry.destroyEntity(entity);

    EXPECT_FALSE(registry.isAlive(entity));
  }
  ```
