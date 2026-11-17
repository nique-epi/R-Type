#pragma once

#include <string>

namespace rtype::game {

/** @brief The mask file of a sheet as it was read, before anything is
 * checked: how the errors name it, and everything it holds. */
struct SheetMaskText {
  std::string source;
  std::string text;
};

}  // namespace rtype::game
