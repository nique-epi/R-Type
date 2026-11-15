#pragma once

#include "PercentConstants.hpp"

namespace rtype::game {

/** @brief What a script may ask of the body it orders besides its sheet and
 * its place: its health, in percent of the health its sheet gives. */
struct SpawnOptions {
  int healthPercent{WHOLE_PERCENT};
};

}  // namespace rtype::game
