---
description: "Error handling — custom per-module exceptions in Exceptions/, never a raw throw std:: nor a hard-coded message"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: Domain errors are raised through the module's custom exceptions, never a raw `throw std::…`

## Goal

A generic `std::` exception thrown from domain code carries no domain information: the caller cannot tell "unknown packet type" apart from another `std::invalid_argument` coming from elsewhere, nor catch *the family* of errors of a module in a single `catch`. A custom hierarchy gives one `catch` per family, centralized and testable messages, and keeps std exceptions from crossing module boundaries.

## Rules

- Each module has an **`Exceptions/`** folder with:
  - a **root exception** for the module, deriving from `std::runtime_error`;
  - **specific subclasses** per error case.
- **Directly throwing a standard library exception is forbidden** (`std::invalid_argument`, `std::runtime_error`, `std::out_of_range`, `std::logic_error`…).
- **No hard-coded message in the `throw`**: the message is built by the exception class, from its parameters or from named constants of the module.
- **Before creating an exception, check it does not already exist**; otherwise extend the existing one rather than multiplying classes.
- The `Exceptions/` files are added to the module's CMake target.
- Writing `throw std::` is the signal that a class is missing in the module's `Exceptions/`.

## Examples

- ❌ **Forbidden**:
  ```cpp
  Room::Room(std::size_t capacity) : capacity_(capacity) {
    if (capacity == 0) {
      throw std::invalid_argument("Room capacity must be strictly positive");
    }
  }
  ```
- ✅ **Instead**:
  ```cpp
  // src/server/Lobby/Exceptions/LobbyException.hpp
  namespace rtype::lobby {

  class LobbyException : public std::runtime_error {
   public:
    explicit LobbyException(const std::string &message);
  };

  class InvalidRoomCapacityException : public LobbyException {
   public:
    explicit InvalidRoomCapacityException(std::size_t capacity);
  };

  }  // namespace rtype::lobby
  ```
  ```cpp
  // Room.cpp
  if (capacity == 0) {
    throw InvalidRoomCapacityException(capacity);
  }
  ```
