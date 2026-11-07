#pragma once

#include <functional>
#include "CurrentAction.hpp"
#include "HurtZone.hpp"
#include "Position.hpp"

namespace rtype::game {

/** @brief An enemy whose hurt zone is checked against the ships: its zone,
 * where it stands, and the action it is in, nullptr for one without
 * CurrentAction. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct HurtingEnemy {
  std::reference_wrapper<const HurtZone> zone;
  Position position{};
  const CurrentAction* current{nullptr};
};

}  // namespace rtype::game
