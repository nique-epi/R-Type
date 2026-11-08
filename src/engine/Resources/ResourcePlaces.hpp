#pragma once

#include <filesystem>

namespace rtype::engine {

/** @brief Where the content of a program comes from: its assets, then the
 * resources it starts. */
struct ResourcePlaces {
  /** @brief The assets folder, the content every program ships. */
  std::filesystem::path assetFolder;
  /** @brief The folder that holds one folder per resource. */
  std::filesystem::path resourceFolder;
};

}  // namespace rtype::engine
