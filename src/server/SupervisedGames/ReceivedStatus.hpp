#pragma once

#include <cstdint>
#include "GameStatus.hpp"

namespace rtype::server {

/** @brief The status a game process sent, and the token and sequence of the
 * datagram that carried it. */
struct ReceivedStatus {
  std::uint64_t token{};
  std::uint16_t sequence{};
  network::GameStatus status;
};

}  // namespace rtype::server
