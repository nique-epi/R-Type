#pragma once

#include <string>

namespace rtype::client {

/** @brief One background file as it was read, before anything is checked. */
struct BackgroundText {
  /** @brief The name of the background: the name of the file, without
   * `.toml`. */
  std::string name;
  /** @brief How the errors name the file: its absolute path. */
  std::string source;
  /** @brief Everything the file holds. */
  std::string text;
};

}  // namespace rtype::client
