#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief The program the server binary runs, as `--mode` chooses it. */
enum class ServerMode : std::uint8_t {
  /** @brief The game server, which hosts games: the default. */
  Server,
  /** @brief The master, which lists the game servers and directs them. */
  Master,
};

}  // namespace rtype::server
