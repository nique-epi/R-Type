#pragma once

#include <functional>

namespace rtype::game {

struct LuauBehaviourState;

/** @brief A function of a script that listens to an event: the behaviour
 * state of the entity that subscribed it, and its reference in the registry
 * of the machine. */
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
struct LuauEventListener {
  std::reference_wrapper<LuauBehaviourState> subscriber;
  int function{0};
};

}  // namespace rtype::game
