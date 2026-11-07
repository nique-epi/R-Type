#pragma once

#include <string>

namespace rtype::game {

/** @brief One file of the content a server plays with, as a client needs it.
 */
struct ContentFile {
  /**
   * @brief Its path inside the assets folder, its folders separated by
   * ASSET_PATH_SEPARATOR: `entities/drifter.toml`, `sprites/entities.png`.
   */
  std::string path;
  /** @brief Every byte of the file. */
  std::string bytes;
};

}  // namespace rtype::game
