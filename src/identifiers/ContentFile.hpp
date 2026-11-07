#pragma once

#include <string>

namespace rtype::identifiers {

/** @brief One file of the content a server plays with, as a client needs it.
 */
struct ContentFile {
  /**
   * @brief Its path inside the assets folder, its folders separated by
   * `/`: `entities/drifter.toml`, `sprites/entities.png`.
   */
  std::string path;
  /** @brief Every byte of the file. */
  std::string bytes;
};

}  // namespace rtype::identifiers
