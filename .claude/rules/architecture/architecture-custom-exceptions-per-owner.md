---
description: "Error handling: the custom exceptions of a library or program in its Exceptions/ folder, never a raw throw std:: nor a hard-coded message"
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: Domain errors are raised through the custom exceptions of their library or program, never a raw `throw std::…`

## Goal

A generic `std::` exception thrown from domain code carries no domain information: the caller cannot tell "unknown packet type" apart from another `std::invalid_argument` coming from elsewhere, nor catch *the family* of errors of a library in a single `catch`. A custom hierarchy gives one `catch` per family, centralized and testable messages, and keeps std exceptions from crossing library boundaries.

## Rules

- Each library and program of `src/`, its owner (`src/engine`, `src/network`, `src/server`…), has an **`Exceptions/`** folder with:
  - a **root exception** for that owner, deriving from `std::runtime_error`;
  - **specific subclasses** per error case.
- **Directly throwing a standard library exception is forbidden** (`std::invalid_argument`, `std::runtime_error`, `std::out_of_range`, `std::logic_error`…).
- **No hard-coded message in the `throw`**: the message is built by the exception class, from its parameters or from named constants of the owner.
- **Before creating an exception, check it does not already exist**; otherwise extend the existing one rather than multiplying classes.
- The `Exceptions/` files are added to the CMake target of that folder.
- Writing `throw std::` is the signal that a class is missing in the `Exceptions/` of the owner.

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
  // src/server/Exceptions/ServerException/ServerException.hpp
  namespace rtype::server {

  class ServerException : public std::runtime_error {
   public:
    explicit ServerException(const std::string &message);
  };

  class InvalidRoomCapacityException : public ServerException {
   public:
    explicit InvalidRoomCapacityException(std::size_t capacity);
  };

  }  // namespace rtype::server
  ```
  ```cpp
  // Room.cpp
  if (capacity == 0) {
    throw InvalidRoomCapacityException(capacity);
  }
  ```
