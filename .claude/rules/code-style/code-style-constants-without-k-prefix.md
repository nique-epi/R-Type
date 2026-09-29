---
description: Constant naming — no k prefix, lowerCamelCase by default, UPPER_SNAKE_CASE tolerated for public header constants
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: No `k` prefix on constants

## Goal

The `k` prefix is a Google convention that adds nothing beyond what `constexpr`, `const` and `enum` already express in the type system. Naming stays direct and uniform.

## Rules

- Constants (`constexpr`, `const`, `enum class` values) are **never prefixed with `k`**.
- Default naming is `lowerCamelCase`: `tickRate`, `maxPlayers`, `epsilon`.
- `UPPER_SNAKE_CASE` is accepted for a **public** constant exposed in a header and shared across modules (`DEFAULT_SERVER_PORT`).
- `enum class` values in `PascalCase`: `PacketType::PlayerInput`.
- Any existing `kFoo` constant is renamed as soon as the file is touched.

## Examples

- ❌ **Forbidden**:
  ```cpp
  constexpr int kTickRate = 60;
  constexpr std::size_t kMaxPlayers = 4;
  ```
- ✅ **Instead**:
  ```cpp
  constexpr int tickRate = 60;
  constexpr std::size_t maxPlayers = 4;

  // Public constant shared across modules
  constexpr std::uint16_t DEFAULT_SERVER_PORT = 4242;
  ```
