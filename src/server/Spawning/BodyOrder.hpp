#pragma once

#include <cstdint>
#include <functional>
#include "EntitySheet.hpp"
#include "Position.hpp"

namespace rtype::server {

/** @brief A body a script asked for, which appears at the end of the tick. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct BodyOrder {
  std::reference_wrapper<const game::EntitySheet> sheet;
  game::Position position{};
  std::uint32_t networkId{};
};

}  // namespace rtype::server
