#pragma once

#include <cstdint>

namespace rtype::server {

/** @brief The program the server binary runs, as `--mode` chooses it. */
enum class ServerMode : std::uint8_t {
  /** @brief A game process, which hosts one game: only a supervisor starts
   * one, with `--mode game`. */
  Game,
  /** @brief The master, which lists the servers and directs them. */
  Master,
  /** @brief The supervisor, which sends the players to games that each run in
   * a process of their own: the default. */
  Supervisor,
};

}  // namespace rtype::server
