#pragma once

#include <string>
#include <variant>

namespace rtype::game {

/** @brief What a field of an LDtk entity holds: a flag, a number, or a text
 * (a string, an enum value or a color such as `#ff8800`). */
using LdtkFieldValue = std::variant<bool, double, std::string>;

}  // namespace rtype::game
