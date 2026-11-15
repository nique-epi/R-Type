#pragma once

#include <cstdint>
#include <functional>
#include "EntitySheet.hpp"
#include "PercentConstants.hpp"
#include "Position.hpp"

namespace rtype::server {

/** @brief A body a script asked for, which appears at the end of the tick,
 * with its health at `healthPercent` percent of what its sheet gives. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct EntityOrder {
  std::reference_wrapper<const game::EntitySheet> sheet;
  game::Position position{};
  std::uint32_t networkId{};
  int healthPercent{game::WHOLE_PERCENT};
};

}  // namespace rtype::server
