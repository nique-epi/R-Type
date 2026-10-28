#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>

namespace rtype::server {

/** @brief What the games module is set up with. */
struct GamesSettings {
  /** @brief The seed the random draws of the first game start from; a seed
   * drawn at random when empty. */
  std::optional<std::uint32_t> seed;
  /** @brief The waves file, read when the module starts. */
  std::filesystem::path wavesFile;
};

}  // namespace rtype::server
