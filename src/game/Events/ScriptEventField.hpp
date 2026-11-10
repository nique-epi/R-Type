#pragma once

#include <string>
#include <variant>

namespace rtype::game {

/** @brief One value a script event carries, under its key: a number, a flag
 * or a text. */
struct ScriptEventField {
  std::string key;
  std::variant<double, bool, std::string> value;

  bool operator==(const ScriptEventField&) const = default;
};

}  // namespace rtype::game
