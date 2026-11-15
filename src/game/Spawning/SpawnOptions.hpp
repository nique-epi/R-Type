#pragma once

#include "PercentConstants.hpp"

namespace rtype::game {

/** @brief What may be asked of a body ordered through IEntityOrders besides
 * its sheet and its place: its health, in percent of the health its sheet
 * gives. */
struct SpawnOptions {
  int healthPercent{WHOLE_PERCENT};
};

}  // namespace rtype::game
