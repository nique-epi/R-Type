#pragma once

#include "ActionKey.hpp"

namespace rtype::game {

/**
 * @brief Component of an enemy that any hit destroys while it is in the
 * action `in`, whatever health it has left: the Drillback, stunned after its
 * charge. Out of that action, a hit costs what it costs any enemy.
 */
struct Fragile {
  ActionKey in{};
};

}  // namespace rtype::game
