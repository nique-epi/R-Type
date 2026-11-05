#pragma once

#include <cstdint>
#include <memory>
#include <vector>
#include "WorldState.hpp"

namespace rtype::server {

/**
 * @brief A world state the server sent a player: its tick, the sequence of the
 * datagram each of its parts went in, and its entities sorted by id, shared
 * with the other players who got the same state.
 */
struct SentState {
  std::uint32_t tick;
  std::vector<std::uint16_t> sequences;
  std::shared_ptr<const std::vector<network::EntityState>> entities;
};

}  // namespace rtype::server
