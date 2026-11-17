#pragma once

#include <cstdint>
#include <functional>
#include "EntitySheet.hpp"
#include "Position.hpp"
#include "SpawnOptions.hpp"

namespace rtype::server {

/** @brief A body the game was asked for, which appears at the end of the
 * tick as its options say. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct EntityOrder {
  std::reference_wrapper<const game::EntitySheet> sheet;
  game::Position position{};
  std::uint32_t networkId{};
  game::SpawnOptions options{};
};

}  // namespace rtype::server
