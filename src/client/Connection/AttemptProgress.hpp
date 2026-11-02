#pragma once

#include <cstdint>

namespace rtype::client {

/** @brief How an attempt to join a server stands after a frame. */
enum class AttemptProgress : std::uint8_t {
  /** @brief The server may still answer. */
  Waiting,
  /** @brief CONNECTION_ATTEMPT_TIMEOUT passed since the attempt began. */
  TimedOut,
};

}  // namespace rtype::client
