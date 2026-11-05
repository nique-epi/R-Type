#pragma once

#include <functional>
#include "IBehaviour.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief Component of a body a behaviour pilots: which behaviour, and what the
 * body remembers for it.
 *
 * The behaviour belongs to a BehaviourRegistry that outlives the body: the
 * World holds both.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct Pilot {
  std::reference_wrapper<const IBehaviour> behaviour;
  /** @brief Where the center of the body was when it appeared. */
  Position origin{};
  /** @brief How long the body has existed; PilotSystem adds each tick. */
  engine::Duration age{};
};

}  // namespace rtype::game
