#pragma once

#include <string>

namespace rtype::game {

/** @brief Why an LDtk file is refused: the file, as the errors show it, and
 * what is wrong in it. */
struct LdtkRefusal {
  std::string file;
  std::string reason;
};

}  // namespace rtype::game
