---
description: Magic values — meaningful addresses, ports, numbers and strings go in named constants, never inline
paths:
  - "**/*.{cpp,hpp,tpp}"
---

# RULE: Addresses, ports, magic numbers and magic strings — NEVER inline, always named constants

Why: the repository's `.clang-tidy` disables `readability-magic-numbers`; this rule is what guards it.

## Rule

1. **Zero magic number / magic string in function bodies**: every meaningful literal (tick rate, speed, buffer size, packet header size, timeout, max player count, asset path, packet type id) gets a **name** as a constant. The test: if you need to read the context to understand what the literal stands for, it must be named.
2. **Where to put them**: at the top of the `.cpp` (anonymous namespace) if the value is local to the file; in a module constants header (`NetworkConstants.hpp`) if it is shared. A shared value is never duplicated.
3. **Address and port**: a default named as a constant, overridden by command-line arguments; never an address written inside a function body.
4. **Consistency within a file**: if one value of the file is a constant, every value of the same kind is.
5. Not magic: `0`, `1`, `-1` in their obvious arithmetic meaning, `nullptr`, `true`/`false`.

## Example

- ❌ **Before (wrong)**:
  ```cpp
  socket_.open(asio::ip::udp::v4());
  socket_.bind({asio::ip::udp::v4(), 4242});
  std::array<std::byte, 1024> buffer{};
  if (bytesReceived < 6) {
    return;
  }
  ```
- ✅ **After (right)**:
  ```cpp
  // NetworkConstants.hpp
  constexpr std::uint16_t DEFAULT_SERVER_PORT = 4242;
  constexpr std::size_t MAX_DATAGRAM_SIZE = 1024;
  constexpr std::size_t PACKET_HEADER_SIZE = 6;
  ```
  ```cpp
  socket_.bind({asio::ip::udp::v4(), port_});
  std::array<std::byte, MAX_DATAGRAM_SIZE> buffer{};
  if (bytesReceived < PACKET_HEADER_SIZE) {
    return;
  }
  ```
