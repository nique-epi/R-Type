#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief The program the server binary runs, as `--mode` chooses it. */
enum class ServerMode : std::uint8_t {
  /** @brief The game server, which hosts games. A supervisor starts one per
   * game; it can also be run alone with `--mode server`. */
  Server,
  /** @brief The master, which lists the game servers and directs them. */
  Master,
  /** @brief The supervisor, which sends the players to games that each run in
   * a process of their own: the default. */
  Supervisor,
};

}  // namespace rtype::server
