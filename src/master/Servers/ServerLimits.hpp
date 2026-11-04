#pragma once

#include <cstdint>

namespace rtype::master {

/** @brief A registered server, with the bounds its heartbeats must keep. */
struct ServerLimits {
  std::int64_t id{};
  std::uint16_t maximumGames{};
  std::uint8_t maximumPlayersPerGame{};
};

}  // namespace rtype::master
