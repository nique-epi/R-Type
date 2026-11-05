#pragma once

#include <string>

namespace rtype::game {

/** @brief One entity sheet as it was read, before anything is checked. */
struct SheetText {
  /** @brief The name of the type: the name of the file, without `.toml`. */
  std::string name;
  /** @brief How the errors name the file: its absolute path. */
  std::string source;
  /** @brief Everything the file holds. */
  std::string text;
};

}  // namespace rtype::game
