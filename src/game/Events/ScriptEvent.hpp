#pragma once

#include <string>
#include <vector>
#include "ScriptEventField.hpp"

namespace rtype::game {

/**
 * @brief Event a script published under a name of its choice, with the values
 * it gave: the scripts that subscribed to that name receive them.
 */
struct ScriptEvent {
  std::string name;
  std::vector<ScriptEventField> fields;

  bool operator==(const ScriptEvent&) const = default;
};

}  // namespace rtype::game
