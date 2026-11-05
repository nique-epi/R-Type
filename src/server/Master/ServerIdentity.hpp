#pragma once

#include <cstdint>
#include <string>

namespace rtype::server {

/** @brief What the server tells the master of itself when it registers. */
struct ServerIdentity {
  /** @brief The name players read, the server-name setting. */
  std::string name;
  /** @brief The UDP port players reach it on. */
  std::uint16_t udpPort{};
  /** @brief How many games it hosts. */
  std::uint16_t maximumGames{};
};

}  // namespace rtype::server
