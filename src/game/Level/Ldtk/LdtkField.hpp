#pragma once

#include <string>
#include "LdtkFieldValue.hpp"

namespace rtype::game {

/** @brief A custom field of an LDtk entity, by the identifier the project
 * gives it. */
struct LdtkField {
  std::string name;
  LdtkFieldValue value;
};

}  // namespace rtype::game
