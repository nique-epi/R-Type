#pragma once

#include <cstddef>

namespace rtype::server {

/** @brief What the game of a game process holds at one moment. */
struct GamesCensus {
  /** @brief The games the process hosts, in progress or not: one in a game
   * process, from its start. */
  std::size_t hosted{0};
  std::size_t inProgress{0};
  /** @brief The players connected to a game, in progress or not. */
  std::size_t players{0};
  /** @brief The entities of the world of the game. */
  std::size_t entities{0};
};

}  // namespace rtype::server
