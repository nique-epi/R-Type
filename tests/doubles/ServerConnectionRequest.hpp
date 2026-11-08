#pragma once

#include <cstdint>

/** @brief What a screen asked of the connection: to start an attempt or to
 * drop it. */
enum class ServerConnectionRequest : std::uint8_t {
  Connect,
  Cancel,
};
