#pragma once

#include "Position.hpp"

namespace rtype::client::testing {

/** @brief The sounds the sheet of the cued Bydo names for its appearance and
 * its death, and where it stands. */
constexpr const char* CUED_APPEAR_SOUND = "sounds/bydo/appear.ogg";
constexpr const char* CUED_DEATH_SOUND = "sounds/bydo/death.ogg";
constexpr game::Position CUED_BYDO_PLACE{.x = 400.0F, .y = 300.0F};

}  // namespace rtype::client::testing
