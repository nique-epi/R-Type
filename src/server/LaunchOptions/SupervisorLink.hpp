#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief What links a game process to the supervisor that started it. */
struct SupervisorLink {
  /** @brief The UDP port of the supervisor, on the loopback. */
  std::uint16_t port{};
  /** @brief The session token of their datagrams; never 0. */
  std::uint64_t token{};
};

}  // namespace rtype::server
