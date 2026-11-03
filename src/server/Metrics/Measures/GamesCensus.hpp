#pragma once

#include <cstddef>

namespace rtype::server {

/** @brief What the games of the server hold at one moment. */
struct GamesCensus {
  /** @brief The games the server hosts, in progress or not. They all exist from
   * the start of the server. */
  std::size_t hosted{0};
  std::size_t inProgress{0};
  /** @brief The players connected to a game, in progress or not. */
  std::size_t players{0};
  /** @brief The entities of the worlds of every game. */
  std::size_t entities{0};
};

}  // namespace rtype::server
