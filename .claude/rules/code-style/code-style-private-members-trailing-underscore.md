---
description: Private members — trailing underscore (name_), never leading; no underscore on public members
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: Trailing underscore for private members

## Goal

The `_identifier` prefix is reserved by the C++ standard in some contexts (global scope, `_` followed by an uppercase letter) and stays ambiguous with C conventions. The `name_` suffix is the chosen convention (Google C++ Style, base of the repository's `.clang-format`). Mixing both betrays code pasted from elsewhere.

## Rules

- **Private and protected** data members carry a **trailing** underscore: `socket_`, `entities_`, `tickRate_`.
- **Never a prefix**: `_socket`, `m_socket`, `mSocket` are forbidden.
- **Public** members have no underscore: aggregate types (`struct Vector2 { float x; float y; };`, plain-data ECS components) expose bare names.
- Applies to every new declaration and every declaration touched during a refactoring.

## Examples

- ❌ **Forbidden**:
  ```cpp
  class UdpServer {
   private:
    asio::ip::udp::socket _socket;
    std::uint16_t m_port;
  };
  ```
- ✅ **Instead**:
  ```cpp
  class UdpServer {
   private:
    asio::ip::udp::socket socket_;
    std::uint16_t port_;
  };

  struct Velocity {
    float x;
    float y;
  };
  ```
