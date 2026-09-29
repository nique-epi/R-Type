---
description: Strict .hpp / .cpp split — the header declares, the .cpp defines
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: A `.hpp` only holds declarations, every definition goes in the `.cpp`

## Goal

A `.hpp` describes the **interface** of a component: skimming it for a few seconds must be enough to understand its API. Keeping method bodies out of headers reduces compile times, avoids transitive dependencies and clearly separates contract from implementation.

## Rules

- A `.hpp` only contains: classes, method signatures, `enum`, `struct`, `using`, template declarations, forward declarations.
- **No logic, no method body** in a `.hpp` — one-line getters and setters included.
- **Limited, justified technical exceptions**:
  - templates that must be defined in the header (or in a `.tpp` included at the end of the header);
  - `constexpr` when the definition is needed at compile time;
  - trivial `= default` / `= delete` methods.
- A header only includes what its **declarations** need; other `#include`s go in the `.cpp`. Prefer a forward declaration when a pointer or reference is enough.
- Applies to every `.hpp` in `src/` and `tests/`.

## Examples

- ❌ **Forbidden** (`Player.hpp`):
  ```cpp
  class Player {
   public:
    int health() const { return health_; }
    void takeDamage(int amount) {
      health_ -= amount;
    }

   private:
    int health_;
  };
  ```
- ✅ **Instead**:
  ```cpp
  // Player.hpp
  class Player {
   public:
    int health() const;
    void takeDamage(int amount);

   private:
    int health_;
  };
  ```
  ```cpp
  // Player.cpp
  #include "Player.hpp"

  int Player::health() const { return health_; }

  void Player::takeDamage(int amount) { health_ -= amount; }
  ```
