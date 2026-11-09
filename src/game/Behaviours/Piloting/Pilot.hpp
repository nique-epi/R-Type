#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include "IBehaviour.hpp"
#include "IBehaviourMemory.hpp"
#include "Position.hpp"
#include "TimeConstants.hpp"

namespace rtype::game {

/**
 * @brief Component of a body a behaviour pilots: which behaviour, the values
 * of its settings, and what the body remembers for it.
 *
 * The behaviour and the values belong to whoever made the body (the
 * EntitySheets of the server), which outlives it: no body owns a copy, so
 * piloting by a built-in behaviour allocates nothing.
 */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct Pilot {
  std::reference_wrapper<const IBehaviour> behaviour;
  /** @brief Where the center of the body was when it appeared. */
  Position origin{};
  /** @brief How long the body has existed; PilotSystem adds each tick. */
  engine::Duration age{};
  /** @brief The values of the settings of the behaviour, in the order it
   * declares them; PilotSystem hands them to it in BehaviourView. */
  std::span<const std::int64_t> settings;
  /** @brief What the behaviour remembers of this body, empty for a behaviour
   * that remembers nothing; it ends with the body. */
  std::shared_ptr<IBehaviourMemory> memory = nullptr;
};

}  // namespace rtype::game
