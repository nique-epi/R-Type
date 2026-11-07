#pragma once

#include <cstddef>
#include "ServerConstants.hpp"
#include "StateEncoding.hpp"

namespace rtype::server {

/**
 * @brief How the world state goes to each player of a game: how many times per
 * second, and in which form.
 */
struct StateSending {
  /** @brief From LOWEST_WORLD_STATES_PER_SECOND to
   * HIGHEST_WORLD_STATES_PER_SECOND; it need not divide the tick rate. */
  std::size_t statesPerSecond{DEFAULT_WORLD_STATES_PER_SECOND};
  StateEncoding encoding{DEFAULT_STATE_ENCODING};
};

}  // namespace rtype::server
