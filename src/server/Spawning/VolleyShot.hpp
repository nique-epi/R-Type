#pragma once

#include <functional>
#include "Position.hpp"
#include "VolleySheet.hpp"

namespace rtype::server {

/** @brief A volley fired at this tick: the center of the enemy that fires
 * it, and the volley, which belongs to the sheet of that enemy. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct VolleyShot {
  game::Position center{};
  std::reference_wrapper<const game::VolleySheet> volley;
};

}  // namespace rtype::server
